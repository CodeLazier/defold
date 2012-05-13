#include "res_tileset.h"

#include <render/render_ddf.h>

#include "../gamesys.h"

namespace dmGameSystem
{
    dmResource::Result AcquireResources(dmPhysics::HContext2D context, dmResource::HFactory factory, const void* buffer, uint32_t buffer_size,
                          TileSetResource* tile_set, const char* filename)
    {
        dmGameSystemDDF::TileSet* tile_set_ddf;
        dmDDF::Result e  = dmDDF::LoadMessage(buffer, buffer_size, &tile_set_ddf);
        if ( e != dmDDF::RESULT_OK )
        {
            return dmResource::RESULT_FORMAT_ERROR;
        }

        dmResource::Result r = dmResource::Get(factory, tile_set_ddf->m_Image, (void**)&tile_set->m_Texture);
        if (r == dmResource::RESULT_OK)
        {
            tile_set->m_TileSet = tile_set_ddf;
            uint16_t width = dmGraphics::GetTextureWidth(tile_set->m_Texture);
            uint16_t height = dmGraphics::GetTextureHeight(tile_set->m_Texture);
            // Check dimensions
            if (width < tile_set_ddf->m_TileWidth || height < tile_set_ddf->m_TileHeight)
            {
                return dmResource::RESULT_INVALID_DATA;
            }
            uint32_t n_hulls = tile_set_ddf->m_ConvexHulls.m_Count;
            tile_set->m_HullCollisionGroups.SetCapacity(n_hulls);
            tile_set->m_HullCollisionGroups.SetSize(n_hulls);
            dmPhysics::HullDesc* hull_descs = new dmPhysics::HullDesc[n_hulls];
            for (uint32_t i = 0; i < n_hulls; ++i)
            {
                dmGameSystemDDF::ConvexHull* hull_ddf = &tile_set_ddf->m_ConvexHulls[i];
                tile_set->m_HullCollisionGroups[i] = dmHashString64(hull_ddf->m_CollisionGroup);
                hull_descs[i].m_Index = (uint16_t)hull_ddf->m_Index;
                hull_descs[i].m_Count = (uint16_t)hull_ddf->m_Count;
            }
            uint32_t n_points = tile_set_ddf->m_ConvexHullPoints.m_Count / 2;
            float recip_tile_width = 1.0f / (tile_set_ddf->m_TileWidth - 1);
            float recip_tile_height = 1.0f / (tile_set_ddf->m_TileHeight - 1);
            float* points = tile_set_ddf->m_ConvexHullPoints.m_Data;
            float* norm_points = new float[n_points * 2];
            for (uint32_t i = 0; i < n_points; ++i)
            {
                norm_points[i*2] = (points[i*2]) * recip_tile_width - 0.5f;
                norm_points[i*2+1] = (points[i*2+1]) * recip_tile_height - 0.5f;
            }
            tile_set->m_HullSet = dmPhysics::NewHullSet2D(context, norm_points, n_points, hull_descs, n_hulls);
            delete [] hull_descs;
            delete [] norm_points;

            uint32_t n_animations = tile_set_ddf->m_Animations.m_Count;
            tile_set->m_AnimationIds.SetCapacity(n_animations);
            for (uint32_t i = 0; i < n_animations; ++i)
            {
                tile_set->m_AnimationIds.Push(dmHashString64(tile_set_ddf->m_Animations[i].m_Id));
            }
        }
        else
        {
            dmDDF::FreeMessage(tile_set_ddf);
        }
        return r;
    }

    void ReleaseResources(dmResource::HFactory factory, TileSetResource* tile_set)
    {
        if (tile_set->m_Texture)
            dmResource::Release(factory, tile_set->m_Texture);

        if (tile_set->m_TileSet)
            dmDDF::FreeMessage(tile_set->m_TileSet);

        if (tile_set->m_HullSet)
            dmPhysics::DeleteHullSet2D(tile_set->m_HullSet);
    }

    dmResource::Result ResTileSetCreate(dmResource::HFactory factory,
            void* context,
            const void* buffer, uint32_t buffer_size,
            dmResource::SResourceDescriptor* resource,
            const char* filename)
    {
        TileSetResource* tile_set = new TileSetResource();

        dmResource::Result r = AcquireResources(((PhysicsContext*)context)->m_Context2D, factory, buffer, buffer_size, tile_set, filename);
        if (r == dmResource::RESULT_OK)
        {
            resource->m_Resource = (void*) tile_set;
        }
        else
        {
            ReleaseResources(factory, tile_set);
            delete tile_set;
        }
        return r;
    }

    dmResource::Result ResTileSetDestroy(dmResource::HFactory factory,
            void* context,
            dmResource::SResourceDescriptor* resource)
    {
        TileSetResource* tile_set = (TileSetResource*) resource->m_Resource;
        ReleaseResources(factory, tile_set);
        delete tile_set;
        return dmResource::RESULT_OK;
    }

    dmResource::Result ResTileSetRecreate(dmResource::HFactory factory,
            void* context,
            const void* buffer, uint32_t buffer_size,
            dmResource::SResourceDescriptor* resource,
            const char* filename)
    {
        TileSetResource* tile_set = (TileSetResource*)resource->m_Resource;
        TileSetResource tmp_tile_set;
        dmResource::Result r = AcquireResources(((PhysicsContext*)context)->m_Context2D, factory, buffer, buffer_size, &tmp_tile_set, filename);
        if (r == dmResource::RESULT_OK)
        {
            ReleaseResources(factory, tile_set);
            tile_set->m_TileSet = tmp_tile_set.m_TileSet;
            tile_set->m_Texture = tmp_tile_set.m_Texture;
            tile_set->m_HullCollisionGroups.Swap(tmp_tile_set.m_HullCollisionGroups);
            tile_set->m_HullSet = tmp_tile_set.m_HullSet;
        }
        else
        {
            ReleaseResources(factory, &tmp_tile_set);
        }
        return r;
    }
}
