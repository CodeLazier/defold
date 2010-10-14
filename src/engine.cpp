#include "engine.h"
#include "engine_private.h"

#include <vectormath/cpp/vectormath_aos.h>

#include <sys/stat.h>

#include <dlib/dstrings.h>
#include <dlib/hash.h>
#include <dlib/profile.h>
#include <dlib/time.h>

#include <sound/sound.h>

#include <render/model_ddf.h>
#include <render/render_ddf.h>
#include <render_debug/debugrenderer.h>

#include <gui/gui.h>

#include <particle/particle.h>

#include <gameobject/gameobject_ddf.h>

#include <gamesys/physics_ddf.h>

#include "physics_debug_render.h"
#include "profile_render.h"

using namespace Vectormath::Aos;

namespace dmEngine
{
    void GetWorldTransform(void* user_data, Point3& position, Quat& rotation)
    {
        if (!user_data)
            return;
        dmGameObject::HInstance instance = (dmGameObject::HInstance)user_data;
        position = dmGameObject::GetWorldPosition(instance);
        rotation = dmGameObject::GetWorldRotation(instance);
    }

    void SetWorldTransform(void* user_data, const Point3& position, const Quat& rotation)
    {
        if (!user_data)
            return;
        dmGameObject::HInstance instance = (dmGameObject::HInstance)user_data;
        dmGameObject::SetPosition(instance, position);
        dmGameObject::SetRotation(instance, rotation);
    }

    void SetObjectModel(void* context, void* gameobject, Quat* rotation,
            Point3* position)
    {
        if (!gameobject) return;
        dmGameObject::HInstance go = (dmGameObject::HInstance) gameobject;
        *position = dmGameObject::GetWorldPosition(go);
        *rotation = dmGameObject::GetWorldRotation(go);
    }

    void Dispatch(dmMessage::Message *message_object, void* user_ptr);
    void DispatchGui(dmMessage::Message *message_object, void* user_ptr);

    Engine::Engine()
    : m_Alive(true)
    , m_MainCollection(0)
    , m_Collections()
    , m_ActiveCollection(0)
    , m_SpawnCount(0)
    , m_LastReloadMTime(0)
    , m_MouseSensitivity(1.0f)
    , m_ShowFPS(true)
    , m_ShowProfile(false)
    , m_WarpTimeStep(false)
    , m_TimeStepFactor(1.0f)
    , m_TimeStepMode(dmEngineDDF::TIME_STEP_MODE_DISCRETE)
    , m_Factory(0x0)
    , m_Font(0x0)
    , m_FontRenderer(0x0)
    , m_SmallFont(0x0)
    , m_SmallFontRenderer(0x0)
    , m_RenderdebugVertexProgram(0x0)
    , m_RenderdebugFragmentProgram(0x0)
    , m_InputContext(0x0)
    , m_GameInputBinding(0x0)
    , m_RenderWorld(0x0)
    , m_RenderPass(0x0)
    {
        m_Register = dmGameObject::NewRegister(Dispatch, this);
        m_Collections.SetCapacity(16, 32);
        m_InputBuffer.SetCapacity(64);

        m_PhysicsContext.m_Debug = false;
        m_EmitterContext.m_Debug = false;
    }

    HEngine New()
    {
        return new Engine();
    }

    void DeleteCollection(Engine* context, const uint32_t* key, dmGameObject::HCollection* collection)
    {
        dmResource::Release(context->m_Factory, *collection);
    }

    void Delete(HEngine engine)
    {
        engine->m_Collections.Iterate<Engine>(DeleteCollection, engine);
        if (engine->m_MainCollection)
            dmResource::Release(engine->m_Factory, engine->m_MainCollection);
        dmGameObject::DeleteRegister(engine->m_Register);

        UnloadBootstrapContent(engine);

        dmSound::Finalize();

        dmInput::DeleteContext(engine->m_InputContext);

        dmRenderDebug::Finalize();
        dmHID::Finalize();

        dmGameObject::Finalize();

        if (engine->m_Factory)
            dmResource::DeleteFactory(engine->m_Factory);

        dmGraphics::DestroyDevice();

        dmProfile::Finalize();

        delete engine;
    }

    bool Init(HEngine engine, int argc, char *argv[])
    {
        const char* project_file;

        if (argc > 1 && argv[argc-1][0] != '-')
            project_file = argv[argc-1];
        else
            project_file = "build/default/content/game.projectc";

        dmConfigFile::HConfig config;
        dmConfigFile::Result config_result = dmConfigFile::Load(project_file, argc, (const char**) argv, &config);
        if (config_result != dmConfigFile::RESULT_OK)
        {
            dmLogError("Unable to load %s", project_file);
            return false;
        }

        dmProfile::Initialize(256, 1024);

        dmGraphics::HDevice device;
        dmGraphics::CreateDeviceParams graphics_params;

        graphics_params.m_DisplayWidth = dmConfigFile::GetInt(config, "display.width", 960);
        graphics_params.m_DisplayHeight = dmConfigFile::GetInt(config, "display.height", 540);
        graphics_params.m_AppTitle = dmConfigFile::GetString(config, "project.title", "TestTitle");
        graphics_params.m_Fullscreen = false;
        graphics_params.m_PrintDeviceInfo = false;

        engine->m_ScreenWidth = graphics_params.m_DisplayWidth;
        engine->m_ScreenHeight = graphics_params.m_DisplayHeight;

        device = dmGraphics::CreateDevice(&argc, argv, &graphics_params);

        dmGraphics::HContext context = dmGraphics::GetContext();

        dmGraphics::EnableState(context, dmGraphics::DEPTH_TEST);
        dmGameObject::Initialize();

        RegisterDDFTypes();

        dmHID::Initialize();

        dmSound::InitializeParams sound_params;
        dmSound::Initialize(config, &sound_params);

        engine->m_RenderWorld = dmRender::NewRenderWorld(100, 100, 0x0);
        engine->m_RenderContext.m_GFXContext = dmGraphics::GetContext();

        engine->m_EmitterContext.m_RenderContext = &engine->m_RenderContext;
        engine->m_EmitterContext.m_MaxEmitterCount = dmConfigFile::GetInt(config, dmParticle::MAX_EMITTER_COUNT_KEY, 0);
        engine->m_EmitterContext.m_MaxParticleCount = dmConfigFile::GetInt(config, dmParticle::MAX_PARTICLE_COUNT_KEY, 0);
        engine->m_EmitterContext.m_Debug = false;
        engine->m_EmitterContext.m_RenderWorld = engine->m_RenderWorld;

        const uint32_t max_resources = 256;

        dmRenderDebug::Initialize(engine->m_RenderWorld);

        dmResource::NewFactoryParams params;
        params.m_MaxResources = max_resources;
        params.m_Flags = RESOURCE_FACTORY_FLAGS_RELOAD_SUPPORT | RESOURCE_FACTORY_FLAGS_HTTP_SERVER;
        params.m_StreamBufferSize = 8 * 1024 * 1024; // We have some *large* textures...!


        dmRender::RenderPassDesc rp_model_desc("model", 0x0, 1, 1000, 1, 0x0, 0x0);
        engine->m_RenderPass = dmRender::NewRenderPass(&rp_model_desc);
        dmRender::AddRenderPass(engine->m_RenderWorld, engine->m_RenderPass);

        engine->m_Factory = dmResource::NewFactory(&params, dmConfigFile::GetString(config, "resource.uri", "build/default/content"));

        dmPhysics::SetDebugRenderer(PhysicsDebugRender::RenderLine);

        float repeat_delay = dmConfigFile::GetFloat(config, "input.repeat_delay", 0.5f);
        float repeat_interval = dmConfigFile::GetFloat(config, "input.repeat_interval", 0.2f);
        engine->m_InputContext = dmInput::NewContext(repeat_delay, repeat_interval);

        dmResource::FactoryResult fact_result;
        dmGameObject::Result res;

        fact_result = dmGameObject::RegisterResourceTypes(engine->m_Factory, engine->m_Register);
        if (fact_result != dmResource::FACTORY_RESULT_OK)
            goto bail;
        fact_result = dmGameSystem::RegisterResourceTypes(engine->m_Factory);
        if (fact_result != dmResource::FACTORY_RESULT_OK)
            goto bail;

        if (dmGameObject::RegisterComponentTypes(engine->m_Factory, engine->m_Register) != dmGameObject::RESULT_OK)
            goto bail;

        res = dmGameSystem::RegisterComponentTypes(engine->m_Factory, engine->m_Register, &engine->m_RenderContext, &engine->m_PhysicsContext, &engine->m_EmitterContext, engine->m_RenderWorld);
        if (res != dmGameObject::RESULT_OK)
            goto bail;

        if (!LoadBootstrapContent(engine, config))
        {
            dmLogWarning("Unable to load bootstrap data.");
            goto bail;
        }

        fact_result = dmResource::Get(engine->m_Factory, dmConfigFile::GetString(config, "bootstrap.main_collection", "logic/main.collectionc"), (void**) &engine->m_MainCollection);
        if (fact_result != dmResource::FACTORY_RESULT_OK)
            goto bail;
        dmGameObject::Init(engine->m_MainCollection);

        engine->m_LastReloadMTime = 0;
        struct stat file_stat;
        if (stat("build/default/content/reload", &file_stat) == 0)
        {
            engine->m_LastReloadMTime = (uint32_t) file_stat.st_mtime;
        }

        dmConfigFile::Delete(config);
        return true;

bail:
        dmConfigFile::Delete(config);
        return false;
    }

    void Reload(HEngine engine)
    {
        struct stat file_stat;
        if (stat("build/default/content/reload", &file_stat) == 0 && engine->m_LastReloadMTime != (uint32_t) file_stat.st_mtime)
        {
            engine->m_LastReloadMTime = (uint32_t) file_stat.st_mtime;
            ReloadResources(engine, "scriptc");
            ReloadResources(engine, "emitterc");
        }
    }

    void ReloadResources(HEngine engine, const char* extension)
    {
        uint32_t type;
        dmResource::FactoryResult r;
        r = dmResource::GetTypeFromExtension(engine->m_Factory, extension, &type);
        assert(r == dmResource::FACTORY_RESULT_OK);
        r = dmResource::ReloadType(engine->m_Factory, type);
        if (r != dmResource::FACTORY_RESULT_OK)
        {
            dmLogWarning("Failed to reload resources with extension \"%s\".", extension);
        }
    }

    void GOActionCallback(uint32_t action_id, dmInput::Action* action, void* user_data)
    {
        dmArray<dmGameObject::InputAction>* input_buffer = (dmArray<dmGameObject::InputAction>*)user_data;
        dmGameObject::InputAction input_action;
        input_action.m_ActionId = action_id;
        input_action.m_Value = action->m_Value;
        input_action.m_Pressed = action->m_Pressed;
        input_action.m_Released = action->m_Released;
        input_action.m_Repeated = action->m_Repeated;
        input_buffer->Push(input_action);
    }

    int32_t Run(HEngine engine)
    {
        const float fps = 60.0f;
        float actual_fps = fps;

        float accumulated_time = 0.0f;

        uint64_t time_stamp = dmTime::GetTime();

        engine->m_Alive = true;
        engine->m_ExitCode = 0;

        while (engine->m_Alive)
        {
            dmProfile::Begin();
            {
                DM_PROFILE(Main, "Frame");

                // We had buffering problems with the output when running the engine inside the editor
                // Flushing stdout/stderr solves this problem.
                fflush(stdout);
                fflush(stderr);

                dmResource::UpdateFactory(engine->m_Factory);
                Reload(engine);

                dmHID::Update();
                dmSound::Update();

                dmHID::KeyboardPacket keybdata;
                dmHID::GetKeyboardPacket(&keybdata);
                if (dmHID::GetKey(&keybdata, dmHID::KEY_ESC))
                {
                    engine->m_Alive = false;
                    break;
                }

                float fixed_dt = 1.0f / fps;
                float dt = fixed_dt;

                dmInput::UpdateBinding(engine->m_GameInputBinding, fixed_dt);

                if (engine->m_WarpTimeStep)
                {
                    float warped_dt = dt * engine->m_TimeStepFactor;
                    switch (engine->m_TimeStepMode)
                    {
                    case dmEngineDDF::TIME_STEP_MODE_CONTINUOUS:
                        dt = warped_dt;
                        break;
                    case dmEngineDDF::TIME_STEP_MODE_DISCRETE:
                        accumulated_time += warped_dt;
                        if (accumulated_time >= fixed_dt)
                        {
                            dt = fixed_dt + accumulated_time - fixed_dt;
                            accumulated_time = 0.0f;
                        }
                        else
                        {
                            dt = 0.0f;
                        }
                        break;
                    default:
                        break;
                    }
                }

                dmGraphics::HContext context = dmGraphics::GetContext();
                dmGraphics::Clear(context, dmGraphics::CLEAR_COLOUR_BUFFER | dmGraphics::CLEAR_DEPTH_BUFFER, 0, 0, 0, 0, 1.0, 0);
                dmGraphics::SetViewport(context, engine->m_ScreenWidth, engine->m_ScreenHeight);

                engine->m_InputBuffer.SetSize(0);
                dmInput::ForEachActive(engine->m_GameInputBinding, GOActionCallback, &engine->m_InputBuffer);
                if (engine->m_InputBuffer.Size() > 0)
                {
                    dmGameObject::HCollection collections[2] = {engine->m_MainCollection, engine->m_ActiveCollection};
                    dmGameObject::DispatchInput(collections, 2, &engine->m_InputBuffer[0], engine->m_InputBuffer.Size());
                }

                uint32_t socket = dmGameObject::GetMessageSocketId(engine->m_Register);
                dmMessage::Dispatch(socket, &Dispatch, engine);

                dmGameObject::UpdateContext update_contexts[2];
                update_contexts[0].m_DT = dt;
                update_contexts[0].m_ViewProj = engine->m_RenderContext.m_ViewProj;
                update_contexts[1].m_DT = fixed_dt;
                update_contexts[1].m_ViewProj = engine->m_RenderContext.m_ViewProj;
                dmGameObject::HCollection collections[2] = {engine->m_ActiveCollection, engine->m_MainCollection};
                dmGameObject::Update(collections, update_contexts, 2);

                dmGameObject::PostUpdate(collections, 2);

                if (engine->m_ShowFPS)
                {
                    uint16_t x = 10;
                    uint16_t y = 40;
                    uint16_t height = 20;
                    char debug_text[32];
                    DM_SNPRINTF(debug_text, sizeof(debug_text), "Target FPS: %.2f", fps);
                    dmRender::FontRendererDrawString(engine->m_FontRenderer, debug_text, x, y, 1.0f, 1.0f, 1.0f, 1.0f);
                    y += height;
                    DM_SNPRINTF(debug_text, sizeof(debug_text), "Actual FPS: %.2f", actual_fps);
                    dmRender::FontRendererDrawString(engine->m_FontRenderer, debug_text, x, y, 1.0f, 1.0f, 1.0f, 1.0f);
                    y += height;
                    DM_SNPRINTF(debug_text, sizeof(debug_text), "dt: %.4f", dt);
                    dmRender::FontRendererDrawString(engine->m_FontRenderer, debug_text, x, y, 1.0f, 1.0f, 1.0f, 1.0f);
                    dmRender::FontRendererFlush(engine->m_FontRenderer);
                }
            }

            dmProfile::End();
            if (engine->m_ShowProfile)
                dmProfileRender::Draw(engine->m_SmallFontRenderer, engine->m_ScreenWidth, engine->m_ScreenHeight);


            dmRender::SetViewProjectionMatrix(engine->m_RenderPass, &engine->m_RenderContext.m_ViewProj);
            dmRenderDebug::Update();
            dmRender::Update(engine->m_RenderWorld, 0.0f);
            dmGraphics::Flip();

            uint64_t new_time_stamp, delta;
            new_time_stamp = dmTime::GetTime();
            delta = new_time_stamp - time_stamp;
            time_stamp = new_time_stamp;

            float actual_dt = delta / 1000000.0f;
            if (actual_dt > 0.0f)
                actual_fps = 1.0f / actual_dt;
            else
                actual_fps = -1.0f;
        }
        return engine->m_ExitCode;
    }

    void Exit(HEngine engine, int32_t code)
    {
        engine->m_Alive = false;
        engine->m_ExitCode = code;
    }

    void Dispatch(dmMessage::Message *message_object, void* user_ptr)
    {
        DM_PROFILE(Game, "Dispatch")

        Engine* self = (Engine*) user_ptr;
        dmGameObject::InstanceMessageData* instance_message_data = (dmGameObject::InstanceMessageData*) message_object->m_Data;

        if (instance_message_data->m_DDFDescriptor == dmEngineDDF::Exit::m_DDFDescriptor)
        {
            dmEngineDDF::Exit* ddf = (dmEngineDDF::Exit*) instance_message_data->m_Buffer;
            dmEngine::Exit(self, ddf->m_Code);
        }
        else if (instance_message_data->m_DDFDescriptor == dmGameObjectDDF::LoadCollection::m_DDFDescriptor)
        {
            dmGameObjectDDF::LoadCollection* ll = (dmGameObjectDDF::LoadCollection*) instance_message_data->m_Buffer;
            ll->m_Collection = (const char*)((uintptr_t)ll + (uintptr_t)ll->m_Collection);
            dmEngine::LoadCollection(self, ll->m_Collection);
        }
        else if (instance_message_data->m_DDFDescriptor == dmGameObjectDDF::UnloadCollection::m_DDFDescriptor)
        {
            dmGameObjectDDF::UnloadCollection* ll = (dmGameObjectDDF::UnloadCollection*) instance_message_data->m_Buffer;
            ll->m_Collection = (const char*)((uintptr_t)ll + (uintptr_t)ll->m_Collection);
            dmEngine::UnloadCollection(self, ll->m_Collection);
        }
        else if (instance_message_data->m_DDFDescriptor == dmGameObjectDDF::ActivateCollection::m_DDFDescriptor)
        {
            dmGameObjectDDF::ActivateCollection* ddf = (dmGameObjectDDF::ActivateCollection*) instance_message_data->m_Buffer;
            ddf->m_Collection = (const char*)((uintptr_t)ddf + (uintptr_t)ddf->m_Collection);
            dmEngine::ActivateCollection(self, ddf->m_Collection);
        }
        else if (instance_message_data->m_DDFDescriptor == dmGameObjectDDF::AcquireInputFocus::m_DDFDescriptor)
        {
            dmGameObjectDDF::AcquireInputFocus* ddf = (dmGameObjectDDF::AcquireInputFocus*) instance_message_data->m_Buffer;
            ddf->m_GameObjectId = (const char*)((uintptr_t)ddf + (uintptr_t)ddf->m_GameObjectId);
            uint32_t id;
            sscanf(ddf->m_GameObjectId, "%X", &id);
            dmGameObject::HCollection collection = self->m_MainCollection;
            dmGameObject::HInstance instance = dmGameObject::GetInstanceFromIdentifier(self->m_MainCollection, id);
            if (!instance && self->m_ActiveCollection != 0)
            {
                instance = dmGameObject::GetInstanceFromIdentifier(self->m_ActiveCollection, id);
                collection = self->m_ActiveCollection;
            }
            if (instance)
            {
                dmGameObject::AcquireInputFocus(collection, instance);
            }
            else
            {
                dmLogWarning("Game object with id %X could not be found when trying to acquire input focus.", id);
            }
        }
        else if (instance_message_data->m_DDFDescriptor == dmGameObjectDDF::ReleaseInputFocus::m_DDFDescriptor)
        {
            dmGameObjectDDF::ReleaseInputFocus* ddf = (dmGameObjectDDF::ReleaseInputFocus*) instance_message_data->m_Buffer;
            ddf->m_GameObjectId = (const char*)((uintptr_t)ddf + (uintptr_t)ddf->m_GameObjectId);
            uint32_t id;
            sscanf(ddf->m_GameObjectId, "%X", &id);
            dmGameObject::HCollection collection = self->m_MainCollection;
            dmGameObject::HInstance instance = dmGameObject::GetInstanceFromIdentifier(self->m_MainCollection, id);
            if (!instance && self->m_ActiveCollection != 0)
            {
                instance = dmGameObject::GetInstanceFromIdentifier(self->m_ActiveCollection, id);
                collection = self->m_ActiveCollection;
            }
            if (instance)
            {
                dmGameObject::ReleaseInputFocus(collection, instance);
            }
        }
        else if (instance_message_data->m_DDFDescriptor == dmGameObjectDDF::GameObjectTransformQuery::m_DDFDescriptor)
        {
            dmGameObject::InstanceMessageData* instance_message_data = (dmGameObject::InstanceMessageData*) message_object->m_Data;
            dmGameObjectDDF::GameObjectTransformQuery* pq = (dmGameObjectDDF::GameObjectTransformQuery*) instance_message_data->m_Buffer;
            pq->m_GameObjectId = (const char*)((uintptr_t)pq + (uintptr_t)pq->m_GameObjectId);
            uint32_t id;
            sscanf(pq->m_GameObjectId, "%X", &id);
            dmGameObject::HInstance instance = dmGameObject::GetInstanceFromIdentifier(self->m_MainCollection, id);
            if (!instance && self->m_ActiveCollection != 0)
                instance = dmGameObject::GetInstanceFromIdentifier(self->m_ActiveCollection, id);
            if (instance)
            {
                const uint32_t offset = sizeof(dmGameObject::InstanceMessageData) + sizeof(dmGameObjectDDF::GameObjectTransformResult);
                char buf[offset + 9];
                dmGameObject::InstanceMessageData* out_instance_message_data = (dmGameObject::InstanceMessageData*)buf;
                out_instance_message_data->m_MessageId = dmHashString32(dmGameObjectDDF::GameObjectTransformResult::m_DDFDescriptor->m_ScriptName);
                out_instance_message_data->m_Instance = instance_message_data->m_Instance;
                out_instance_message_data->m_Component = 0xff;
                out_instance_message_data->m_DDFDescriptor = dmGameObjectDDF::GameObjectTransformResult::m_DDFDescriptor;
                dmGameObjectDDF::GameObjectTransformResult* result = (dmGameObjectDDF::GameObjectTransformResult*)(buf + sizeof(dmGameObject::InstanceMessageData));
                DM_SNPRINTF(&buf[offset], 9, "%s", pq->m_GameObjectId);
                result->m_GameObjectId = (const char*)sizeof(dmGameObjectDDF::GameObjectTransformResult);
                result->m_Position = dmGameObject::GetPosition(instance);
                result->m_Rotation = dmGameObject::GetRotation(instance);
                uint32_t reply_socket_id = dmGameObject::GetReplyMessageSocketId(self->m_Register);
                uint32_t reply_message_id = dmGameObject::GetMessageId(self->m_Register);
                dmMessage::Post(reply_socket_id, reply_message_id, buf, dmGameObject::INSTANCE_MESSAGE_MAX);
            }
            else
            {
                dmLogWarning("Could not find instance with id %d (%s).", id, pq->m_GameObjectId);
            }
        }
        else if (instance_message_data->m_DDFDescriptor == dmRenderDDF::DrawString::m_DDFDescriptor)
        {
            dmRenderDDF::DrawString* dt = (dmRenderDDF::DrawString*) instance_message_data->m_Buffer;
            dt->m_Text = (const char*) ((uintptr_t) dt + (uintptr_t) dt->m_Text);
            dmRender::FontRendererDrawString(self->m_FontRenderer, dt->m_Text, dt->m_Position.getX(), dt->m_Position.getY(), 0.0f, 0.0f, 1.0f, 1.0f);
        }
        else if (instance_message_data->m_DDFDescriptor == dmRenderDDF::DrawLine::m_DDFDescriptor)
        {
            dmRenderDDF::DrawLine* dl = (dmRenderDDF::DrawLine*) instance_message_data->m_Buffer;
            dmRenderDebug::Line3D(dl->m_StartPoint, dl->m_EndPoint, dl->m_Color);
        }
        else if (instance_message_data->m_DDFDescriptor == dmEngineDDF::SetTimeStep::m_DDFDescriptor)
        {
            dmEngineDDF::SetTimeStep* ddf = (dmEngineDDF::SetTimeStep*)instance_message_data->m_Buffer;
            self->m_TimeStepFactor = ddf->m_Factor;
            self->m_TimeStepMode = ddf->m_Mode;
            self->m_WarpTimeStep = true;
        }
        else if (instance_message_data->m_DDFDescriptor == dmPhysicsDDF::RayCastRequest::m_DDFDescriptor)
        {
            uint32_t id = dmGameObject::GetIdentifier(instance_message_data->m_Instance);
            dmGameObject::HCollection collection = self->m_MainCollection;
            dmGameObject::HInstance instance = dmGameObject::GetInstanceFromIdentifier(self->m_MainCollection, id);
            if (!instance && self->m_ActiveCollection != 0)
            {
                instance = dmGameObject::GetInstanceFromIdentifier(self->m_ActiveCollection, id);
                collection = self->m_ActiveCollection;
            }
            dmPhysicsDDF::RayCastRequest* ddf = (dmPhysicsDDF::RayCastRequest*)instance_message_data->m_Buffer;
            dmGameSystem::RequestRayCast(collection, instance_message_data->m_Instance, ddf->m_From, ddf->m_To, ddf->m_Mask);
        }
        else
        {
            if (instance_message_data->m_MessageId == dmHashString32("toggle_profile"))
            {
                self->m_ShowProfile = !self->m_ShowProfile;
            }
            else if (instance_message_data->m_MessageId == dmHashString32("reset_time_step"))
            {
                self->m_WarpTimeStep = false;
            }
            else
            {
                dmLogError("Unknown message: %s\n", instance_message_data->m_DDFDescriptor->m_Name);
            }
        }
    }

    void DispatchGui(dmMessage::Message *message_object, void* user_ptr)
    {
        DM_PROFILE(Game, "DispatchGui")

        Engine* self = (Engine*) user_ptr;

        dmGui::MessageData* gui_message = (dmGui::MessageData*) message_object->m_Data;
        dmGameObject::HInstance instance = (dmGameObject::HInstance) dmGui::GetSceneUserData(gui_message->m_Scene);

        dmGameObject::InstanceMessageData data;
        data.m_Component = 0xff;
        data.m_DDFDescriptor = 0x0;
        data.m_MessageId = gui_message->m_MessageId;
        data.m_Instance = instance;
        uint32_t socket = dmGameObject::GetReplyMessageSocketId(self->m_Register);
        uint32_t message_id = dmGameObject::GetMessageId(self->m_Register);
        dmMessage::Post(socket, message_id, &data, sizeof(dmGameObject::InstanceMessageData));
    }

    void LoadCollection(HEngine engine, const char* collection_name)
    {
        dmGameObject::HCollection collection;
        dmResource::FactoryResult r = dmResource::Get(engine->m_Factory, collection_name, (void**) &collection);
        if (r == dmResource::FACTORY_RESULT_OK)
        {
            engine->m_Collections.Put(dmHashString32(collection_name), collection);
        }
    }

    void UnloadCollection(HEngine engine, const char* collection_name)
    {
        uint32_t collection_id = dmHashString32(collection_name);
        dmGameObject::HCollection* collection = engine->m_Collections.Get(collection_id);
        if (collection != 0x0)
        {
            dmResource::Release(engine->m_Factory, *collection);
            engine->m_Collections.Erase(collection_id);
            if (*collection == engine->m_ActiveCollection)
                engine->m_ActiveCollection = 0;
        }
    }

    void ActivateCollection(HEngine engine, const char* collection_name)
    {
        uint32_t collection_id = dmHashString32(collection_name);
        dmGameObject::HCollection* collection = engine->m_Collections.Get(collection_id);
        if (collection != 0x0)
        {
            dmGameObject::Init(*collection);
            engine->m_ActiveCollection = *collection;
        }
    }

    void RegisterDDFTypes()
    {
        dmGameSystem::RegisterDDFTypes();

        dmGameObject::RegisterDDFType(dmEngineDDF::Exit::m_DDFDescriptor);
        dmGameObject::RegisterDDFType(dmEngineDDF::SetTimeStep::m_DDFDescriptor);
        dmGameObject::RegisterDDFType(dmRenderDDF::DrawString::m_DDFDescriptor);
        dmGameObject::RegisterDDFType(dmRenderDDF::DrawLine::m_DDFDescriptor);
        dmGameObject::RegisterDDFType(dmRender::SetRenderColor::m_DDFDescriptor);
        dmGameObject::RegisterDDFType(dmGameObjectDDF::LoadCollection::m_DDFDescriptor);
        dmGameObject::RegisterDDFType(dmGameObjectDDF::UnloadCollection::m_DDFDescriptor);
        dmGameObject::RegisterDDFType(dmGameObjectDDF::ActivateCollection::m_DDFDescriptor);
        dmGameObject::RegisterDDFType(dmGameObjectDDF::AcquireInputFocus::m_DDFDescriptor);
        dmGameObject::RegisterDDFType(dmGameObjectDDF::ReleaseInputFocus::m_DDFDescriptor);
        dmGameObject::RegisterDDFType(dmGameObjectDDF::GameObjectTransformQuery::m_DDFDescriptor);
        dmGameObject::RegisterDDFType(dmGameObjectDDF::GameObjectTransformResult::m_DDFDescriptor);

        dmGui::RegisterDDFType(dmGameObjectDDF::GameObjectTransformQuery::m_DDFDescriptor);
        dmGui::RegisterDDFType(dmGameObjectDDF::GameObjectTransformResult::m_DDFDescriptor);
    }

    bool LoadBootstrapContent(HEngine engine, dmConfigFile::HConfig config)
    {
        dmResource::FactoryResult fact_error = dmResource::Get(engine->m_Factory, dmConfigFile::GetString(config, "bootstrap.font", "fonts/VeraMoBd.fontc"), (void**) &engine->m_Font);
        if (fact_error != dmResource::FACTORY_RESULT_OK)
        {
            return false;
        }

        fact_error = dmResource::Get(engine->m_Factory, dmConfigFile::GetString(config, "bootstrap.small_font", "fonts/VeraMoBd2.fontc"), (void**) &engine->m_SmallFont);
        if (fact_error != dmResource::FACTORY_RESULT_OK)
        {
            return false;
        }

        engine->m_FontRenderer = dmRender::NewFontRenderer(engine->m_Font, engine->m_RenderWorld, engine->m_ScreenWidth,
            engine->m_ScreenHeight, 2048 * 4);
        engine->m_SmallFontRenderer = dmRender::NewFontRenderer(engine->m_SmallFont, engine->m_RenderWorld, engine->m_ScreenWidth,
            engine->m_ScreenHeight, 2048 * 4);

        // debug renderer needs vertex/fragment programs, load them here (perhaps they need to be moved into render/debug?)
        engine->m_RenderdebugVertexProgram = dmGraphics::CreateVertexProgram((const void*) ::DEBUG_ARBVP, ::DEBUG_ARBVP_SIZE);
        assert(engine->m_RenderdebugVertexProgram);
        dmRenderDebug::SetVertexProgram(engine->m_RenderdebugVertexProgram);

        engine->m_RenderdebugFragmentProgram = dmGraphics::CreateFragmentProgram((const void*) ::DEBUG_ARBFP, ::DEBUG_ARBFP_SIZE);
        assert(engine->m_RenderdebugFragmentProgram);
        dmRenderDebug::SetFragmentProgram(engine->m_RenderdebugFragmentProgram);

        const char* gamepads = dmConfigFile::GetString(config, "bootstrap.gamepads", "input/default.gamepadsc");
        dmInputDDF::GamepadMaps* gamepad_maps_ddf;
        fact_error = dmResource::Get(engine->m_Factory, gamepads, (void**)&gamepad_maps_ddf);
        if (fact_error != dmResource::FACTORY_RESULT_OK)
            return false;
        dmInput::RegisterGamepads(engine->m_InputContext, gamepad_maps_ddf);
        dmResource::Release(engine->m_Factory, gamepad_maps_ddf);

        const char* game_input_binding = dmConfigFile::GetString(config, "bootstrap.game_binding", "input/game.input_bindingc");
        dmInputDDF::InputBinding* ddf;
        fact_error = dmResource::Get(engine->m_Factory, game_input_binding, (void**)&ddf);
        if (fact_error != dmResource::FACTORY_RESULT_OK)
            return false;
        engine->m_GameInputBinding = dmInput::NewBinding(engine->m_InputContext, ddf);
        dmResource::Release(engine->m_Factory, ddf);
        if (!engine->m_GameInputBinding)
        {
            dmLogError("Unable to load game input context, did you forget to specify %s in the config file?", "bootstrap.game_config");
            return false;
        }

        return true;
    }

    void UnloadBootstrapContent(HEngine engine)
    {
        if (engine->m_FontRenderer)
            dmRender::DeleteFontRenderer(engine->m_FontRenderer);
        if (engine->m_Font)
            dmResource::Release(engine->m_Factory, engine->m_Font);
        if (engine->m_SmallFontRenderer)
            dmRender::DeleteFontRenderer(engine->m_SmallFontRenderer);
        if (engine->m_SmallFont)
            dmResource::Release(engine->m_Factory, engine->m_SmallFont);

        if (engine->m_RenderdebugVertexProgram)
            dmGraphics::DestroyVertexProgram(engine->m_RenderdebugVertexProgram);
        if (engine->m_RenderdebugFragmentProgram)
            dmGraphics::DestroyFragmentProgram(engine->m_RenderdebugFragmentProgram);

        if (engine->m_GameInputBinding)
            dmInput::DeleteBinding(engine->m_GameInputBinding);

        if (engine->m_RenderPass)
            dmRender::DeleteRenderPass(engine->m_RenderPass);
        if (engine->m_RenderWorld)
            dmRender::DeleteRenderWorld(engine->m_RenderWorld);
    }
}

