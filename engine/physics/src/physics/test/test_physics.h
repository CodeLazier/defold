#ifndef PHYSICS_TEST_PHYSICS_H
#define PHYSICS_TEST_PHYSICS_H

#include <stdint.h>
#include <gtest/gtest.h>
#include "../physics.h"
#include "../physics_2d.h"
#include "../physics_3d.h"


struct VisualObject
{
    VisualObject();

    Vectormath::Aos::Point3 m_Position;
    Vectormath::Aos::Quat   m_Rotation;
    int                     m_CollisionCount;
    uint16_t                m_FirstCollisionGroup;
};

void GetWorldTransform(void* visual_object, Vectormath::Aos::Point3& position, Vectormath::Aos::Quat& rotation);
void SetWorldTransform(void* visual_object, const Vectormath::Aos::Point3& position, const Vectormath::Aos::Quat& rotation);
bool CollisionCallback(void* user_data_a, uint16_t group_a, void* user_data_b, uint16_t group_b, void* user_data);
bool ContactPointCallback(const dmPhysics::ContactPoint& contact_point, void* user_data);

static const float PHYSICS_SCALE = 0.5f;

template<typename T>
class PhysicsTest : public ::testing::Test
{
protected:

    virtual void SetUp()
    {
        dmPhysics::NewContextParams context_params = dmPhysics::NewContextParams();
        context_params.m_Scale = PHYSICS_SCALE;
        m_Context = (*m_Test.m_NewContextFunc)(context_params);
        dmPhysics::NewWorldParams world_params;
        world_params.m_GetWorldTransformCallback = GetWorldTransform;
        world_params.m_SetWorldTransformCallback = SetWorldTransform;
        m_World = (*m_Test.m_NewWorldFunc)(m_Context, world_params);
        m_CollisionCount = 0;
        m_ContactPointCount = 0;
        m_StepWorldContext.m_DT = 1.0f / 60.0f;
        m_StepWorldContext.m_CollisionCallback = CollisionCallback;
        m_StepWorldContext.m_CollisionUserData = &m_CollisionCount;
        m_StepWorldContext.m_ContactPointCallback = ContactPointCallback;
        m_StepWorldContext.m_ContactPointUserData = &m_ContactPointCount;
    }

    virtual void TearDown()
    {
        (*m_Test.m_DeleteWorldFunc)(m_Context, m_World);
        (*m_Test.m_DeleteContextFunc)(m_Context);
    }

    typename T::ContextType m_Context;
    typename T::WorldType m_World;
    T m_Test;
    dmPhysics::StepWorldContext m_StepWorldContext;
    int m_CollisionCount;
    int m_ContactPointCount;
};

template<typename T>
struct Funcs
{
    typedef typename T::ContextType (*NewContextFunc)(const dmPhysics::NewContextParams& params);
    typedef void (*DeleteContextFunc)(typename T::ContextType context);
    typedef typename T::WorldType (*NewWorldFunc)(typename T::ContextType context, const dmPhysics::NewWorldParams& params);
    typedef void (*DeleteWorldFunc)(typename T::ContextType context, typename T::WorldType world);
    typedef void (*StepWorldFunc)(typename T::WorldType world, const dmPhysics::StepWorldContext& context);
    typedef void (*SetCollisionCallbackFunc)(typename T::WorldType world, dmPhysics::CollisionCallback callback, void* user_data);
    typedef void (*SetContactPointCallbackFunc)(typename T::WorldType world, dmPhysics::ContactPointCallback callback, void* user_data);
    typedef void (*DrawDebugFunc)(typename T::WorldType world);
    typedef typename T::CollisionShapeType (*NewBoxShapeFunc)(typename T::ContextType context, const Vectormath::Aos::Vector3& half_extents);
    typedef typename T::CollisionShapeType (*NewSphereShapeFunc)(typename T::ContextType context, float radius);
    typedef typename T::CollisionShapeType (*NewCapsuleShapeFunc)(typename T::ContextType context, float radius, float height);
    typedef typename T::CollisionShapeType (*NewConvexHullShapeFunc)(typename T::ContextType context, const float* vertices, uint32_t vertex_count);
    typedef void (*DeleteCollisionShapeFunc)(typename T::CollisionShapeType shape);
    typedef typename T::CollisionObjectType (*NewCollisionObjectFunc)(typename T::WorldType world, const dmPhysics::CollisionObjectData& data, typename T::CollisionShapeType* shapes, uint32_t shape_count);
    typedef typename T::CollisionObjectType (*NewCollisionObjectFunc2)(typename T::WorldType world, const dmPhysics::CollisionObjectData& data, typename T::CollisionShapeType* shapes, Vectormath::Aos::Vector3* translations, Vectormath::Aos::Quat* rotations, uint32_t shape_count);
    typedef void (*DeleteCollisionObjectFunc)(typename T::WorldType world, typename T::CollisionObjectType collision_object);
    typedef uint32_t (*GetCollisionShapesFunc)(typename T::CollisionObjectType collision_object, typename T::CollisionShapeType* out_buffer, uint32_t buffer_size);
    typedef void (*SetCollisionObjectUserDataFunc)(typename T::CollisionObjectType collision_object, void* user_data);
    typedef void* (*GetCollisionObjectUserDataFunc)(typename T::CollisionObjectType collision_object);
    typedef void (*ApplyForceFunc)(typename T::ContextType context, typename T::CollisionObjectType collision_object, const Vectormath::Aos::Vector3& force, const Vectormath::Aos::Point3& position);
    typedef Vectormath::Aos::Vector3 (*GetTotalForceFunc)(typename T::ContextType context, typename T::CollisionObjectType collision_object);
    typedef Vectormath::Aos::Point3 (*GetWorldPositionFunc)(typename T::ContextType context, typename T::CollisionObjectType collision_object);
    typedef Vectormath::Aos::Quat (*GetWorldRotationFunc)(typename T::ContextType context, typename T::CollisionObjectType collision_object);
    typedef Vectormath::Aos::Vector3 (*GetLinearVelocityFunc)(typename T::ContextType context, typename T::CollisionObjectType collision_object);
    typedef Vectormath::Aos::Vector3 (*GetAngularVelocityFunc)(typename T::ContextType context, typename T::CollisionObjectType collision_object);
    typedef bool (*IsEnabledFunc)(typename T::CollisionObjectType collision_object);
    typedef void (*SetEnabledFunc)(typename T::WorldType world, typename T::CollisionObjectType collision_object, bool enabled);
    typedef bool (*IsSleepingFunc)(typename T::CollisionObjectType collision_object);
    typedef void (*RequestRayCastFunc)(typename T::WorldType world, const dmPhysics::RayCastRequest& request);
    typedef void (*SetDebugCallbacks)(typename T::ContextType context, const dmPhysics::DebugCallbacks& callbacks);
    typedef void (*ReplaceShapeFunc)(typename T::ContextType context, typename T::CollisionShapeType old_shape, typename T::CollisionShapeType new_shape);
};

struct Test3D
{
    Test3D();
    ~Test3D();

    typedef dmPhysics::HContext3D ContextType;
    typedef dmPhysics::HWorld3D WorldType;
    typedef dmPhysics::HCollisionObject3D CollisionObjectType;
    typedef dmPhysics::HCollisionShape3D CollisionShapeType;

    Funcs<Test3D>::NewContextFunc                   m_NewContextFunc;
    Funcs<Test3D>::DeleteContextFunc                m_DeleteContextFunc;
    Funcs<Test3D>::NewWorldFunc                     m_NewWorldFunc;
    Funcs<Test3D>::DeleteWorldFunc                  m_DeleteWorldFunc;
    Funcs<Test3D>::StepWorldFunc                    m_StepWorldFunc;
    Funcs<Test3D>::SetCollisionCallbackFunc         m_SetCollisionCallbackFunc;
    Funcs<Test3D>::SetContactPointCallbackFunc      m_SetContactPointCallbackFunc;
    Funcs<Test3D>::DrawDebugFunc                    m_DrawDebugFunc;
    Funcs<Test3D>::NewBoxShapeFunc                  m_NewBoxShapeFunc;
    Funcs<Test3D>::NewSphereShapeFunc               m_NewSphereShapeFunc;
    Funcs<Test3D>::NewCapsuleShapeFunc              m_NewCapsuleShapeFunc;
    Funcs<Test3D>::NewConvexHullShapeFunc           m_NewConvexHullShapeFunc;
    Funcs<Test3D>::DeleteCollisionShapeFunc         m_DeleteCollisionShapeFunc;
    Funcs<Test3D>::NewCollisionObjectFunc           m_NewCollisionObjectFunc;
    Funcs<Test3D>::NewCollisionObjectFunc2          m_NewCollisionObjectFunc2;
    Funcs<Test3D>::DeleteCollisionObjectFunc        m_DeleteCollisionObjectFunc;
    Funcs<Test3D>::GetCollisionShapesFunc           m_GetCollisionShapesFunc;
    Funcs<Test3D>::SetCollisionObjectUserDataFunc   m_SetCollisionObjectUserDataFunc;
    Funcs<Test3D>::GetCollisionObjectUserDataFunc   m_GetCollisionObjectUserDataFunc;
    Funcs<Test3D>::ApplyForceFunc                   m_ApplyForceFunc;
    Funcs<Test3D>::GetTotalForceFunc                m_GetTotalForceFunc;
    Funcs<Test3D>::GetWorldPositionFunc             m_GetWorldPositionFunc;
    Funcs<Test3D>::GetWorldRotationFunc             m_GetWorldRotationFunc;
    Funcs<Test3D>::GetLinearVelocityFunc            m_GetLinearVelocityFunc;
    Funcs<Test3D>::GetAngularVelocityFunc           m_GetAngularVelocityFunc;
    Funcs<Test3D>::IsEnabledFunc                    m_IsEnabledFunc;
    Funcs<Test3D>::SetEnabledFunc                   m_SetEnabledFunc;
    Funcs<Test3D>::IsSleepingFunc                   m_IsSleepingFunc;
    Funcs<Test3D>::RequestRayCastFunc               m_RequestRayCastFunc;
    Funcs<Test3D>::SetDebugCallbacks                m_SetDebugCallbacksFunc;
    Funcs<Test3D>::ReplaceShapeFunc                 m_ReplaceShapeFunc;

    float*      m_Vertices;
    uint32_t    m_VertexCount;
    float       m_PolygonRadius;
    bool        m_Is3D;
};

struct Test2D
{
    Test2D();
    ~Test2D();

    typedef dmPhysics::HContext2D ContextType;
    typedef dmPhysics::HWorld2D WorldType;
    typedef dmPhysics::HCollisionObject2D CollisionObjectType;
    typedef dmPhysics::HCollisionShape2D CollisionShapeType;

    Funcs<Test2D>::NewContextFunc                   m_NewContextFunc;
    Funcs<Test2D>::DeleteContextFunc                m_DeleteContextFunc;
    Funcs<Test2D>::NewWorldFunc                     m_NewWorldFunc;
    Funcs<Test2D>::DeleteWorldFunc                  m_DeleteWorldFunc;
    Funcs<Test2D>::StepWorldFunc                    m_StepWorldFunc;
    Funcs<Test2D>::DrawDebugFunc                    m_DrawDebugFunc;
    Funcs<Test2D>::NewBoxShapeFunc                  m_NewBoxShapeFunc;
    Funcs<Test2D>::NewSphereShapeFunc               m_NewSphereShapeFunc;
    Funcs<Test2D>::NewCapsuleShapeFunc              m_NewCapsuleShapeFunc;
    Funcs<Test2D>::NewConvexHullShapeFunc           m_NewConvexHullShapeFunc;
    Funcs<Test2D>::DeleteCollisionShapeFunc         m_DeleteCollisionShapeFunc;
    Funcs<Test2D>::NewCollisionObjectFunc           m_NewCollisionObjectFunc;
    Funcs<Test2D>::NewCollisionObjectFunc2          m_NewCollisionObjectFunc2;
    Funcs<Test2D>::DeleteCollisionObjectFunc        m_DeleteCollisionObjectFunc;
    Funcs<Test2D>::GetCollisionShapesFunc           m_GetCollisionShapesFunc;
    Funcs<Test2D>::SetCollisionObjectUserDataFunc   m_SetCollisionObjectUserDataFunc;
    Funcs<Test2D>::GetCollisionObjectUserDataFunc   m_GetCollisionObjectUserDataFunc;
    Funcs<Test2D>::ApplyForceFunc                   m_ApplyForceFunc;
    Funcs<Test2D>::GetTotalForceFunc                m_GetTotalForceFunc;
    Funcs<Test2D>::GetWorldPositionFunc             m_GetWorldPositionFunc;
    Funcs<Test2D>::GetWorldRotationFunc             m_GetWorldRotationFunc;
    Funcs<Test2D>::GetLinearVelocityFunc            m_GetLinearVelocityFunc;
    Funcs<Test2D>::GetAngularVelocityFunc           m_GetAngularVelocityFunc;
    Funcs<Test2D>::IsEnabledFunc                    m_IsEnabledFunc;
    Funcs<Test2D>::SetEnabledFunc                   m_SetEnabledFunc;
    Funcs<Test2D>::IsSleepingFunc                   m_IsSleepingFunc;
    Funcs<Test2D>::RequestRayCastFunc               m_RequestRayCastFunc;
    Funcs<Test2D>::SetDebugCallbacks                m_SetDebugCallbacksFunc;
    Funcs<Test2D>::ReplaceShapeFunc                 m_ReplaceShapeFunc;

    float*      m_Vertices;
    uint32_t    m_VertexCount;
    float       m_PolygonRadius;
    bool        m_Is3D;
};

#endif // PHYSICS_TEST_PHYSICS_H
