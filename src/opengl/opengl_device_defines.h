#ifndef OPENGL_DEVICE_DEFINES_H
#define OPENGL_DEVICE_DEFINES_H

#ifdef __linux__
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#elif defined (__MACH__)

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#elif defined (_WIN32)
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <win32/glut.h>

#else
#error "Platform not supported."
#endif

typedef uint32_t HGFXVertexProgram;
typedef uint32_t HGFXFragmentProgram;
typedef struct SGFXHContext* GFXHContext;
typedef struct SGFXHDevice* GFXHDevice;
typedef struct SGFXHTexture* GFXHTexture;

// Primitive types
#define GFX_DEVICE_PRIMITIVE_POINTLIST           (GL_POINTS)
#define GFX_DEVICE_PRIMITIVE_LINES               (GL_LINES)
#define GFX_DEVICE_PRIMITIVE_LINE_LOOP           (GL_LINE_LOOP)
#define GFX_DEVICE_PRIMITIVE_LINE_STRIP          (-1ul)
#define GFX_DEVICE_PRIMITIVE_TRIANGLES           (GL_TRIANGLES)
#define GFX_DEVICE_PRIMITIVE_TRIANGLE_STRIP      (GL_TRIANGLE_STRIP)
#define GFX_DEVICE_PRIMITIVE_TRIANGLE_FAN        (GL_TRIANGLE_FAN)

// Clear flags
#define GFX_DEVICE_CLEAR_COLOURUFFER            (GL_COLOR_BUFFER_BIT)
#define GFX_DEVICE_CLEAR_DEPTHBUFFER            (GL_DEPTH_BUFFER_BIT)
#define GFX_DEVICE_CLEAR_STENCILBUFFER          (GL_STENCIL_BUFFER_BIT)

#define GFX_DEVICE_MATRIX_TYPE_WORLD            (123ul)
#define GFX_DEVICE_MATRIX_TYPE_VIEW             (321ul)
#define GFX_DEVICE_MATRIX_TYPE_PROJECTION       (GL_PROJECTION)

// Render states
#define GFX_DEVICE_STATE_DEPTH_TEST             (GL_DEPTH_TEST)
#define GFX_DEVICE_STATE_ALPHA_TEST             (GL_ALPHA_TEST)

// Types
#define GFX_DEVICE_TYPE_BYTE                    (GL_BYTE)
#define GFX_DEVICE_TYPE_UNSIGNED_BYTE           (GL_UNSIGNED_BYTE)
#define GFX_DEVICE_TYPE_SHORT                   (GL_SHORT)
#define GFX_DEVICE_TYPE_UNSIGNED_SHORT          (GL_UNSIGNED_SHORT)
#define GFX_DEVICE_TYPE_INT                     (GL_INT)
#define GFX_DEVICE_TYPE_UNSIGNED_INT            (GL_UNSIGNED_INT)
#define GFX_DEVICE_TYPE_FLOAT                   (GL_FLOAT)

// Texture format
#define GFX_DEVICE_TEXTURE_FORMAT_ALPHA         (GL_ALPHA)
#define GFX_DEVICE_TEXTURE_FORMAT_RGBA          (GL_RGBA)
#define GFX_DEVICE_TEXTURE_FORMAT_RGB_DXT1      (GL_COMPRESSED_RGB_S3TC_DXT1_EXT)
#define GFX_DEVICE_TEXTURE_FORMAT_RGBA_DXT1     (GL_COMPRESSED_RGBA_S3TC_DXT1_EXT)
#define GFX_DEVICE_TEXTURE_FORMAT_RGBA_DXT1     (GL_COMPRESSED_RGBA_S3TC_DXT1_EXT)
#define GFX_DEVICE_TEXTURE_FORMAT_RGBA_DXT3     (GL_COMPRESSED_RGBA_S3TC_DXT3_EXT)
#define GFX_DEVICE_TEXTURE_FORMAT_RGBA_DXT5     (GL_COMPRESSED_RGBA_S3TC_DXT5_EXT)

#endif // OPENGL_DEVICE_DEFINES_H 
