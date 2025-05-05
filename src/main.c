#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <whereami.h>

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

  // Loading game resources

  char resourcePath[256], *resourceName;
  {
    int dirnameLength;
    wai_getExecutablePath(resourcePath,
                          sizeof(resourcePath) / sizeof(*resourcePath),
                          &dirnameLength);
    strcpy(resourcePath + dirnameLength, "/../res/");
    resourceName = resourcePath + dirnameLength + strlen("/../res/");
  }
  strcpy(resourceName, "shaders/arrow.frag");

  // Window creation

  GLFWwindow *window =
      glfwCreateWindow(WIDTH, HEIGHT, resourcePath, NULL, NULL);
  glfwMakeContextCurrent(window);

  glfwSetKeyCallback(window, key_callback);

  gladLoadGL(glfwGetProcAddress);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    glClearColor(0.7f, 0.9f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
  }

  glfwTerminate();

  return 0;
}