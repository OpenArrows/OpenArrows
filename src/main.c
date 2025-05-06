#define GLFW_INCLUDE_NONE
#include "gl-debug.h"
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <glad/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const float TILE_COUNT = 32.f;

GLuint winWidth = 800, winHeight = 600;

// Static resources

static const unsigned char grid_vert_spv[] = {
#embed "shaders/grid.vert.spv"
};

static const unsigned char grid_frag_spv[] = {
#embed "shaders/grid.frag.spv"
};

static const unsigned char arrow_comp_spv[] = {
#embed "shaders/arrow.comp.spv"
};

// Game state

float scale = 1.0f;
vec2 cameraOffset = {0.0f, 0.0f};

// Input handlers

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mode) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
}

double dragOffsetX, dragOffsetY;

bool wheelPressed = 0;
static void mouse_button_callback(GLFWwindow *window, int button, int action,
                                  int mods) {
  if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
    wheelPressed = !(action == GLFW_RELEASE);
    if (action == GLFW_PRESS) {
      double mouseX, mouseY;
      glfwGetCursorPos(window, &mouseX, &mouseY);
      dragOffsetX = cameraOffset[0] + mouseX / (double)winWidth;
      dragOffsetY = cameraOffset[1] + mouseY / (double)winHeight;
    }
  }
}

static void cursor_position_callback(GLFWwindow *window, double xpos,
                                     double ypos) {
  if (!wheelPressed)
    return;
  cameraOffset[0] = dragOffsetX - xpos / (double)winWidth;
  cameraOffset[1] = dragOffsetY - ypos / (double)winHeight;
}

double scroll = 0.f;
static void scroll_callback(GLFWwindow *window, double xoffset,
                            double yoffset) {
  scroll += yoffset;
}

static void framebuffer_size_callback(GLFWwindow *window, int width,
                                      int height) {
  winWidth = width;
  winHeight = height;
  glViewport(0, 0, width, height);
}

int main(void) {
  // GL initialization

  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifndef NDEBUG
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);

  glfwSetErrorCallback(gl_error_callback);
#endif

  // Window creation

  GLFWwindow *window =
      glfwCreateWindow(winWidth, winHeight, "Democracy Arrows", NULL, NULL);
  glfwMakeContextCurrent(window);

  glfwSetKeyCallback(window, key_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetCursorPosCallback(window, cursor_position_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  gladLoadGL(glfwGetProcAddress);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Enable debug features

#ifndef NDEBUG
  GLint flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gl_debug_output, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL,
                          GL_TRUE);
  }
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

  GLuint gridFrag = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderBinary(1, &gridFrag, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB,
                 grid_frag_spv, sizeof(grid_frag_spv));
  glSpecializeShaderARB(gridFrag, "main", 0, NULL, NULL);

  GLuint gridProgram = glCreateProgram();
  glAttachShader(gridProgram, gridVert);
  glAttachShader(gridProgram, gridFrag);
  glLinkProgram(gridProgram);

  // Buffers

  GLuint uboTransform;
  glGenBuffers(1, &uboTransform);

  glBindBuffer(GL_UNIFORM_BUFFER, uboTransform);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(mat4), NULL, GL_STATIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboTransform);

  mat4 view;

  GLuint arrowComp = glCreateShader(GL_COMPUTE_SHADER);
  glShaderBinary(1, &arrowComp, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB,
                 arrow_comp_spv, sizeof(arrow_comp_spv));
  glSpecializeShaderARB(arrowComp, "main", 0, NULL, NULL);

  // Main game loop

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    if (scroll != 0.f)
      scale /= powf(1.2f, (float)scroll);
    scroll = 0.f;

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // TODO: Use 2D transformations instead of 3D
    glBindBuffer(GL_UNIFORM_BUFFER, uboTransform);
    glm_mat4_identity(view);
    vec3 viewport = {1.0f * TILE_COUNT,
                     (float)winHeight / (float)winWidth * TILE_COUNT, 1.0f};
    glm_scale(view, viewport);
    vec3 cameraOffset3 = {cameraOffset[0], cameraOffset[1], 0.0f};
    glm_translate(view, cameraOffset3);
    glm_mat4_scale(view, scale);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), view[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glUseProgram(gridProgram);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
  }

  glfwTerminate();

  return 0;
}