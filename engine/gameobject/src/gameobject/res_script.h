#ifndef DM_GAMEOBJECT_RES_SCRIPT_H
#define DM_GAMEOBJECT_RES_SCRIPT_H

#include <stdint.h>

#include <resource/resource.h>

namespace dmGameObject
{
    dmResource::Result ResScriptCreate(dmResource::HFactory factory,
                                             void* context,
                                             const void* buffer, uint32_t buffer_size,
                                             dmResource::SResourceDescriptor* resource,
                                             const char* filename);

    dmResource::Result ResScriptDestroy(dmResource::HFactory factory,
                                              void* context,
                                              dmResource::SResourceDescriptor* resource);

    dmResource::Result ResScriptRecreate(dmResource::HFactory factory,
                                               void* context,
                                               const void* buffer, uint32_t buffer_size,
                                               dmResource::SResourceDescriptor* resource,
                                               const char* filename);
}

#endif // DM_GAMEOBJECT_RES_SCRIPT_H
