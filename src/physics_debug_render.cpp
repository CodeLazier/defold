#include "physics_debug_render.h"

#include <ddf/ddf.h>
#include <render/render.h>
#include <render/mesh_ddf.h>
#include <render/debug_renderer.h>

namespace PhysicsDebugRender
{
    void RenderLine(Vectormath::Aos::Point3 p0, Vectormath::Aos::Point3 p1, Vectormath::Aos::Vector4 color)
    {
        dmRender::Line3D(p0, p1, color);
    }
}
