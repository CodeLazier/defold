#include <string.h>
#include <vectormath/cpp/vectormath_aos.h>

#include "graphics_device.h"
#include "opengl_device.h"
#include <assert.h>

#ifdef __linux__
#include <GL/glext.h>

#elif defined (__MACH__)

#elif defined (_WIN32)
#include "win32/glext.h"

#else
#error "Platform not supported."
#endif

using namespace Vectormath::Aos;

SGFXHContext gcontext;

GFXHContext GFXGetContext()
{
    return (GFXHContext)&gcontext;
}

GFXHDevice GFXCreateDevice(int* argc, char** argv, GFXSCreateDeviceParams *params )
{
    assert(params);

    glutInitWindowSize(params->m_DisplayWidth, params->m_DisplayHeight);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInit(argc, argv);

    glutCreateWindow(params->m_AppTitle);

    return 0;
}

void GFXClear(GFXHContext context, uint32_t flags, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, float depth, uint32_t stencil)
{
    assert(context);

    float r = ((float)red)/255.0f;
    float g = ((float)green)/255.0f;
    float b = ((float)blue)/255.0f;
    float a = ((float)alpha)/255.0f;
    glClearColor(r, g, b, a);

    glClearDepth(depth);
    glClearStencil(stencil);
    glClear(flags);
}

void GFXFlip()
{
    glutSwapBuffers();
}

void GFXDraw(GFXHContext context, GFXPrimitiveType primitive_type, int32_t first, int32_t count )
{
    assert(context);

    glDrawArrays(primitive_type, first, count);
}

void GFXSetVertexStream(GFXHContext context, uint16_t stream, uint16_t size, GFXType type, uint16_t stride, const void* vertex_buffer)
{
    glEnableVertexAttribArray(stream);
    glVertexAttribPointer(stream, size, type, false, stride, vertex_buffer);
}

void GFXDisableVertexStream(GFXHContext context, uint16_t stream)
{
    glDisableVertexAttribArray(stream);
}

void GFXDrawElements(GFXHContext context, GFXPrimitiveType prim_type, uint32_t count, GFXType type, const void* index_buffer)
{
    glDrawElements(prim_type, count, type, index_buffer);
}

static uint32_t GFXCreateProgram(GLenum type, const void* program, uint32_t program_size)
{
    glEnable(type);
    GLuint shader[1];
    glGenProgramsARB(1, shader);
    glBindProgramARB(type, shader[0]);
    glProgramStringARB(type, GL_PROGRAM_FORMAT_ASCII_ARB, program_size, program);
    glDisable(type);
    return shader[0];
}

HGFXVertexProgram GFXCreateVertexProgram(const void* program, uint32_t program_size)
{
    return GFXCreateProgram(GL_VERTEX_PROGRAM_ARB, program, program_size);
}

HGFXFragmentProgram GFXCreateFragmentProgram(const void* program, uint32_t program_size)
{
    return GFXCreateProgram(GL_FRAGMENT_PROGRAM_ARB, program, program_size);
}

static void GFXSetProgram(GLenum type, uint32_t program)
{
    glEnable(type);
    glBindProgramARB(type, program);
}

void GFXSetVertexProgram(GFXHContext context, HGFXVertexProgram program)
{
    GFXSetProgram(GL_VERTEX_PROGRAM_ARB, program);
}

void GFXSetFragmentProgram(GFXHContext context, HGFXFragmentProgram program)
{
    GFXSetProgram(GL_FRAGMENT_PROGRAM_ARB, program);
}

void GFXSetViewport(GFXHContext context, int width, int height, float field_of_view, float z_near, float z_far)
{
    assert(context);

    float aspect_ratio = (float) width / (float) height;

    Matrix4 mat;
    mat = Matrix4::perspective(field_of_view, aspect_ratio, z_near, z_far);

    GFXSetMatrix(context, GFX_MATRIX_TYPE_PROJECTION, &mat);
    glViewport(0, 0, width, height);
}

void GFXSetMatrix(GFXHContext context, GFXMatrixMode matrix_mode, const Matrix4* matrix)
{
    assert(context);
    assert(matrix);

    SGFXHContext* context_t = (SGFXHContext*)context;

    if (matrix_mode == GFX_DEVICE_MATRIX_TYPE_PROJECTION)
    {
        glMatrixMode(matrix_mode);
        glLoadMatrixf((float*)matrix);
    }
    else if (matrix_mode == GFX_DEVICE_MATRIX_TYPE_VIEW)
    {
        context_t->m_ViewMatrix = *matrix;
    }
    else if (matrix_mode == GFX_DEVICE_MATRIX_TYPE_WORLD)
    {
        glMatrixMode(GL_MODELVIEW);
        Matrix4 res = context_t->m_ViewMatrix * (*matrix);
        glLoadMatrixf((float*)&res);
    }
}

void GFXSetTexture(GFXHContext context, GFXHTexture t)
{
    assert(context);
    assert(t);

    SGFXHTexture* tex_h = (SGFXHTexture*)t;
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, tex_h->m_Texture);

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );


}


GFXHTexture GFXCreateTexture(const char* file)
{
    SGFXHTexture* tex = new SGFXHTexture;

    nv::DirectDrawSurface dds(file);
//    dds.printInfo();

    glGenTextures( 1, &tex->m_Texture );
    glBindTexture(GL_TEXTURE_2D, tex->m_Texture);

    int offset = 0;
    for (int i = 0; i < dds.mipmapCount(); ++i)
    {
        dds.mipmap(&tex->m_Image, 0, i);

        int width = tex->m_Image.width();
        int height = tex->m_Image.height();

        char* pixels = (char*)tex->m_Image.pixels();
        int size = ((width+3)/4)*((height+3)/4)*8;

        glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)&pixels[offset]);

        offset += size;
    }

    return (GFXHTexture)tex;
}

void GFXDestroyTexture(GFXHTexture t)
{
    assert(t);
    SGFXHTexture* tex = (SGFXHTexture*)t;

    delete tex;
}

void GFXEnableState(GFXHContext context, GFXRenderState state)
{
    assert(context);

    glEnable(state);
}

void GFXDisableState(GFXHContext context, GFXRenderState state)
{
    assert(context);

    glDisable(state);
}
