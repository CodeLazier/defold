#ifndef __GRAPHICS_DEVICE_OPENGL__
#define __GRAPHICS_DEVICE_OPENGL__

#include <nvtt/nvtt.h>
#include <nvimage/Image.h>
#include <nvimage/DirectDrawSurface.h>
#include "opengl_device_defines.h"

extern GFXHContext g_context;

struct SGFXHTexture
{
    GLuint      m_Texture;
};

struct SGFXHDevice
{

};

struct SGFXHContext
{

    Vectormath::Aos::Matrix4 m_ViewMatrix;
};

#endif // __GRAPHICS_DEVICE_OPENGL__
