#define GLFW_INCLUDE_NONE
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

  // GL resources initialization

  unsigned int compute = glCreateShader(GL_COMPUTE_SHADER);
  glShaderBinary(1, &compute, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB,
                 arrow_comp_spv, sizeof(arrow_comp_spv));
  glSpecializeShaderARB(compute, "main", 0, NULL, NULL);
  glCompileShader(compute);

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

    glClearColor(0.7f, 0.9f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
  }

  glfwTerminate();

  return 0;
}