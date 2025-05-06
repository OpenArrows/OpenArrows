#include <glad/gl.h>

void gl_error_callback(GLint error, const GLchar *description);

void gl_debug_output(GLenum source, GLenum type, unsigned int id,
                     GLenum severity, GLsizei length, const char *message,
                     const void *userParam);