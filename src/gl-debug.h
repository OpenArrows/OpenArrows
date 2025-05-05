#include <glad/gl.h>

void _checkShaderCompileErrors(GLuint shader, const char *file, int line);
void _checkProgramCompileErrors(GLuint shader, const char *file, int line);

#ifndef NDEBUG
#define checkShaderCompileErrors(shader)                                       \
  _checkShaderCompileErrors(shader, __FILE__, __LINE__)
#define checkProgramCompileErrors(shader)                                      \
  _checkProgramCompileErrors(shader, __FILE__, __LINE__)
#else
#define checkShaderCompileErrors(shader)
#define checkProgramCompileErrors(shader)
#endif

void gl_error_callback(GLint error, const GLchar *description);

void gl_debug_output(GLenum source, GLenum type, unsigned int id,
                     GLenum severity, GLsizei length, const char *message,
                     const void *userParam);