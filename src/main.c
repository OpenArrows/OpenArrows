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

  // Buffers

  GLfloat vertices[] = {
      1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f,
      0.f, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 1.f,
  };

  GLuint indices[] = {
      0, 1, 3, 1, 2, 3,
  };

  GLuint vbo, vao, ebo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);
  glEnableVertexAttribArray(0);

  // Shaders

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

  GLuint gridProgram = glCreateProgram();
  glAttachShader(gridProgram, gridVert);
  glAttachShader(gridProgram, gridFrag);
  glLinkProgram(gridProgram);
  checkProgramLinkErrors(gridProgram);

  GLuint arrowComp = glCreateShader(GL_COMPUTE_SHADER);
  glShaderBinary(1, &arrowComp, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB,
                 arrow_comp_spv, sizeof(arrow_comp_spv));
  glSpecializeShaderARB(arrowComp, "main", 0, NULL, NULL);
  glCompileShader(arrowComp);
  checkShaderCompileErrors(arrowComp);

  // Main game loop

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(gridProgram);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
  }

  glfwTerminate();

  return 0;
}