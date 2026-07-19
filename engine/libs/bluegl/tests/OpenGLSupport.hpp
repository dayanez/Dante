
#ifndef __BLUE_GL_OPENGL_SUPPORT_HPP__
#define __BLUE_GL_OPENGL_SUPPORT_HPP__

namespace bluegl {
namespace gl {

typedef void* OpenGLContext;

/**
 * Creates a new OpenGL context.
 *
 * @return an OpenGLContext object that represents the newly created
 *         OpenGL context or nullptr if the context cannot be created
 */
OpenGLContext createOpenGLContext();

/**
 * Makes the specified context the current OpenGL context on the
 * current thread.
 *
 * @param context the OpenGLContext to become current
 */
void setCurrentOpenGLContext(OpenGLContext context);

/**
 * Destroys the specified OpenGL context.
 *
 * @param context the OpenGLContext to destroy
 */
void destroyOpenGLContext(OpenGLContext context);

}; // namespace gl
}; // namespace bluegl

#endif // __BLUE_GL_OPENGL_SUPPORT_HPP__
