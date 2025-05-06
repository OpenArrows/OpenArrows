#include "gl-debug.h"
#include <stdio.h>
#include <stdlib.h>

void gl_error_callback(GLint error, const GLchar *description) {
  fprintf(stderr, "GL error (%d): %s\n", error, description);
  exit(-1);
}

void gl_debug_output(GLenum source, GLenum type, unsigned int id,
                     GLenum severity, GLsizei length, const char *message,
                     const void *userParam) {
  const char *label;
  switch (type) {
  case GL_DEBUG_TYPE_ERROR:
    label = " error";
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    label = " deprecated behaviour";
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    label = " undefined behaviour";
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    label = " portability notice";
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    label = " performance notice";
    break;
  case GL_DEBUG_TYPE_MARKER:
    label = " marker";
    break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
    label = " push group";
    break;
  case GL_DEBUG_TYPE_POP_GROUP:
    label = " pop group";
    break;
  case GL_DEBUG_TYPE_OTHER:
    label = "";
    break;
  }

  const char *prefix;
  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    prefix = "[HIGH] ";
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    prefix = "[MEDIUM] ";
    break;
  case GL_DEBUG_SEVERITY_LOW:
    prefix = "[LOW] ";
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    prefix = "";
    break;
  }

  fprintf(stderr, "%sGL%s: %s\n", prefix, label, message);
}