
#ifndef TNT_DANTE_BACKEND_OPENGL_NULLGLES_H
#define TNT_DANTE_BACKEND_OPENGL_NULLGLES_H

/*
 * This is used for debugging (stubbing out GLES calls)
 *
 * Simply uncomment the line
 *   using namespace nullgles;
 *
 * below.
 *
 */

namespace dante::backend {
namespace nullgles {

inline void glScissor(GLint, GLint, GLsizei, GLsizei) { }
inline void glViewport(GLint, GLint, GLsizei, GLsizei) { }
inline void glDepthRangef(GLfloat, GLfloat) { }
inline void glClearColor(GLfloat, GLfloat, GLfloat, GLfloat) { }
inline void glClearStencil(GLint) { }
inline void glClearDepthf(GLfloat) { }

inline void glBindBufferBase(GLenum, GLuint, GLuint) { }
inline void glBindVertexArray (GLuint) { }
inline void glBindTexture (GLenum, GLuint)   { }
inline void glBindBuffer (GLenum, GLuint) { }
inline void glBindFramebuffer (GLenum, GLuint)   { }
inline void glBindRenderbuffer (GLenum, GLuint) { }
inline void glUseProgram (GLuint)   { }

inline void glBufferData(GLenum, GLsizeiptr, const void *, GLenum) { }
inline void glBufferSubData(GLenum, GLintptr, GLsizeiptr, const void *) { }
inline void glCompressedTexSubImage2D(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const void *) { }
inline void glTexSubImage2D(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const void *) { }
inline void glGenerateMipmap(GLenum) { }

inline void glVertexAttribPointer(GLuint, GLint, GLenum, GLboolean, GLsizei, const void *) { }
inline void glDisableVertexAttribArray (GLuint) { }
inline void glEnableVertexAttribArray (GLuint) { }
inline void glDisable (GLenum cap) { }
inline void glEnable (GLenum cap) { }
inline void glCullFace (GLenum mode) { }
inline void glBlendFunc (GLenum, GLenum) { }

inline void glInvalidateFramebuffer (GLenum, GLsizei, const GLenum *) { }
inline void glInvalidateSubFramebuffer (GLenum, GLsizei, const GLenum *, GLint, GLint, GLsizei, GLsizei);

inline void glSamplerParameteri (GLuint, GLenum, GLint) { }
inline void glSamplerParameterf (GLuint, GLenum, GLfloat) { }

inline void glFramebufferRenderbuffer (GLenum, GLenum, GLenum, GLuint) { }
inline void glRenderbufferStorageMultisample (GLenum, GLsizei, GLenum, GLsizei, GLsizei) { }
inline void glRenderbufferStorage (GLenum, GLenum, GLsizei, GLsizei) { }

inline void glClear(GLbitfield) { }
inline void glDrawRangeElements(GLenum, GLuint, GLuint, GLsizei, GLenum, const void *)  { }
inline void glBlitFramebuffer (GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum) { }
inline void glReadPixels (GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, void *) { }

inline GLenum glGetError() { return GL_NO_ERROR; }

} // namespace nullgles

// turn GLES calls defined above into no-ops
//using namespace nullgles;

} // namespace dante::backend

#endif // TNT_DANTE_BACKEND_OPENGL_NULLGLES_H
