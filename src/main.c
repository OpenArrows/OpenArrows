#define GLFW_INCLUDE_NONE
#include "gl-debug.h"
#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const GLuint WIDTH = 800, HEIGHT = 600;

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mode) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
}

static const unsigned char grid_vert_spv[] = {
#embed "shaders/grid.vert.spv"
};

static const unsigned char grid_frag_spv[] = {
#embed "shaders/grid.frag.spv"
};

static const unsigned char arrow_comp_spv[] = {
#embed "shaders/arrow.comp.spv"
};

int main(void) {
  // GL initialization

  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Window creation

  GLFWwindow *window =
      glfwCreateWindow(WIDTH, HEIGHT, "Democracy Arrows", NULL, NULL);
  glfwMakeContextCurrent(window);

  glfwSetKeyCallback(window, key_callback);

  gladLoadGL(glfwGetProcAddress);

  // Enable debug features

#ifndef NDEBUG
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);

  GLint flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gl_debug_output, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL,
                          GL_TRUE);
  }

  glfwSetErrorCallback(gl_error_callback);
#endif

  // GL resources initialization

  GLuint gridVert = glCreateShader(GL_VERTEX_SHADER);
  glShaderBinary(1, &gridVert, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB,
                 grid_vert_spv, sizeof(grid_vert_spv));
  glSpecializeShaderARB(gridVert, "main", 0, NULL, NULL);
  glCompileShader(gridVert);
  checkShaderCompileErrors(gridVert);

  GLuint gridFrag = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderBinary(1, &gridFrag, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB,
                 grid_frag_spv, sizeof(grid_frag_spv));
  glSpecializeShaderARB(gridFrag, "main", 0, NULL, NULL);
  glCompileShader(gridFrag);
  checkShaderCompileErrors(gridFrag);

  GLuint compute = glCreateShader(GL_COMPUTE_SHADER);
  glShaderBinary(1, &compute, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB,
                 arrow_comp_spv, sizeof(arrow_comp_spv));
  glSpecializeShaderARB(compute, "main", 0, NULL, NULL);
  glCompileShader(compute);
  checkShaderCompileErrors(compute);

  GLint status;
  GLchar infoLog[1024];
  glGetShaderiv(compute, GL_COMPILE_STATUS, &status);
  if (!status) {
    glGetShaderInfoLog(compute, sizeof(infoLog), NULL, infoLog);
    fprintf(stderr, "Shader compilation failed: %s\n", infoLog);
    exit(-1);
  }

  // Main game loop

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
  }

  glfwTerminate();

  return 0;
}