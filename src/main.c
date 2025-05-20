#define GLFW_INCLUDE_NONE
#include "game-map.h"
#include "gl-debug.h"
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <glad/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char APP_NAME[] = "OpenArrows";

// TODO: use something like static `powf`
const float MIN_SCALE =
    1.0f / 1.2f / 1.2f / 1.2f / 1.2f / 1.2f / 1.2f / 1.2f / 1.2f;
const float MAX_SCALE = 1.0f * 1.2f * 1.2f * 1.2f * 1.2f * 1.2f * 1.2f;

const float TILE_COUNT = 32.f;

int arrowGroups[][5] = {{1, 2, 0, 0, 0}};

GLuint winWidth = 800, winHeight = 600;

// Static resources

static const unsigned char grid_vert_spv[] = {
#embed "shaders/grid.vert.spv"
};

static const unsigned char grid_frag_spv[] = {
#embed "shaders/grid.frag.spv"
};

static const unsigned char arrow_vert_spv[] = {
#embed "shaders/arrow.vert.spv"
};

static const unsigned char arrow_frag_spv[] = {
#embed "shaders/arrow.frag.spv"
};

static const unsigned char arrow_comp_spv[] = {
#embed "shaders/arrow.comp.spv"
};

static const unsigned char rect_vert_spv[] = {
#embed "shaders/rect.vert.spv"
};

static const unsigned char rect_frag_spv[] = {
#embed "shaders/rect.frag.spv"
};

static const int ARROW_ATLAS_WIDTH = 2048;
static const int ARROW_ATLAS_HEIGHT = 2048;
static const unsigned char arrow_atlas[] = {
#embed "textures/arrows/atlas.dat"
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
      dragOffsetY = cameraOffset[1] + mouseY / (double)winWidth;
    }
  }
}

static void cursor_position_callback(GLFWwindow *window, double xpos,
                                     double ypos) {
  if (!wheelPressed)
    return;
  cameraOffset[0] = dragOffsetX - xpos / (double)winWidth;
  cameraOffset[1] = dragOffsetY - ypos / (double)winWidth;
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
      glfwCreateWindow(winWidth, winHeight, APP_NAME, NULL, NULL);
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
      1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f,
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

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
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

  GLuint arrowVert = glCreateShader(GL_VERTEX_SHADER);
  glShaderBinary(1, &arrowVert, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB,
                 arrow_vert_spv, sizeof(arrow_vert_spv));
  glSpecializeShaderARB(arrowVert, "main", 0, NULL, NULL);

  GLuint arrowFrag = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderBinary(1, &arrowFrag, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB,
                 arrow_frag_spv, sizeof(arrow_frag_spv));
  glSpecializeShaderARB(arrowFrag, "main", 0, NULL, NULL);

  GLuint arrowProgram = glCreateProgram();
  glAttachShader(arrowProgram, arrowVert);
  glAttachShader(arrowProgram, arrowFrag);
  glLinkProgram(arrowProgram);

  GLuint arrowComp = glCreateShader(GL_COMPUTE_SHADER);
  glShaderBinary(1, &arrowComp, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB,
                 arrow_comp_spv, sizeof(arrow_comp_spv));
  glSpecializeShaderARB(arrowComp, "main", 0, NULL, NULL);

  GLuint arrowComputeProgram = glCreateProgram();
  glAttachShader(arrowComputeProgram, arrowComp);
  glLinkProgram(arrowComputeProgram);

  GLuint rectVert = glCreateShader(GL_VERTEX_SHADER);
  glShaderBinary(1, &rectVert, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB,
                 rect_vert_spv, sizeof(rect_vert_spv));
  glSpecializeShaderARB(rectVert, "main", 0, NULL, NULL);

  GLuint rectFrag = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderBinary(1, &rectFrag, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB,
                 rect_frag_spv, sizeof(rect_frag_spv));
  glSpecializeShaderARB(rectFrag, "main", 0, NULL, NULL);

  GLuint rectProgram = glCreateProgram();
  glAttachShader(rectProgram, rectVert);
  glAttachShader(rectProgram, rectFrag);
  glLinkProgram(rectProgram);

  // Buffers

  mat4 view, projection;

  GLuint uboTransform;
  glGenBuffers(1, &uboTransform);

  glBindBuffer(GL_UNIFORM_BUFFER, uboTransform);
  glBufferData(GL_UNIFORM_BUFFER,
               sizeof(view) + sizeof(projection), // FIXME: is sizeof safe here?
               NULL, GL_STATIC_DRAW);

  glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboTransform);

  mat4 gridTransform;

  GLuint uboGrid;
  glGenBuffers(1, &uboGrid);

  glBindBuffer(GL_UNIFORM_BUFFER, uboGrid);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(gridTransform), NULL, GL_STATIC_DRAW);

  glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboGrid);

  GLuint ssboPassID;
  glGenBuffers(1, &ssboPassID);

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboPassID);
  glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLint), NULL, GL_DYNAMIC_DRAW);

  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssboPassID);

  GLuint uboUITransform;
  glGenBuffers(1, &uboUITransform);

  glBindBuffer(GL_UNIFORM_BUFFER, uboUITransform);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(vec2) * 2, NULL, GL_STATIC_DRAW);

  glBindBufferBase(GL_UNIFORM_BUFFER, 5, uboUITransform);

  GLuint uboUIRect;
  glGenBuffers(1, &uboUIRect);

  glBindBuffer(GL_UNIFORM_BUFFER, uboUIRect);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(vec4) + sizeof(vec2), NULL,
               GL_STATIC_DRAW);

  glBindBufferBase(GL_UNIFORM_BUFFER, 6, uboUIRect);

  // Textures

  GLuint arrowAtlas;
  glGenTextures(1, &arrowAtlas);

  glBindTexture(GL_TEXTURE_2D, arrowAtlas);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ARROW_ATLAS_WIDTH, ARROW_ATLAS_HEIGHT,
               0, GL_RGBA, GL_UNSIGNED_BYTE, arrow_atlas);
  glGenerateMipmap(GL_TEXTURE_2D);

  // Game state

  GameMap map = {1};
  map.map.bufferIndex = 2;
  map.state.bufferIndex = 3;
  map_init(&map);

  map.map.chunks[0].x = 0;
  map.map.chunks[0].y = 1;
  for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE; i++) {
    map.map.chunks[0].arrows[i].type = i;
  }
  map_sync(&map);

  // Main game loop

  GLint passId = 0;

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    if (scroll != 0.f) {
      scale *= powf(1.2f, (float)scroll);
      if (scale < MIN_SCALE) {
        scale = MIN_SCALE;
      } else if (scale > MAX_SCALE) {
        scale = MAX_SCALE;
      }
      // TODO: Adjust the camera position so that the scale origin is at the
      // mouse cursor
    }
    scroll = 0.;

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // TODO: Maybe use 2D transformations instead of 3D?
    glBindBuffer(GL_UNIFORM_BUFFER, uboTransform);

    glm_mat4_identity(view);
    glm_translate(view, (vec3){-cameraOffset[0] * TILE_COUNT,
                               -cameraOffset[1] * TILE_COUNT, 0.0f});
    glm_scale(view, (vec3){scale, scale, 1.0f});
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(view), view[0]);

    // Viewspace unit vec2(1.0, 1.0) is a single cell
    glm_mat4_identity(projection);
    glm_scale(projection, (vec3){1.0f, -1.0f, 1.0f});
    glm_translate(projection, (vec3){-0.5f, -0.5f, 0.0f});
    glm_scale(projection,
              (vec3){1.0f / TILE_COUNT,
                     (float)winWidth / (float)winHeight / TILE_COUNT, 1.0f});
    glm_mat4_scale(projection, 2.0f);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(view), sizeof(projection),
                    projection[0]);

    glUseProgram(arrowProgram);
    glBindTexture(GL_TEXTURE_2D, arrowAtlas);
    glBindVertexArray(vao);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 256);

    glBindBuffer(GL_UNIFORM_BUFFER, uboGrid);

    glm_mat4_identity(gridTransform);
    glm_scale(gridTransform, (vec3){1.0f / scale, 1.0f / scale, 1.0f});
    glm_translate(gridTransform, (vec3){cameraOffset[0] * TILE_COUNT,
                                        cameraOffset[1] * TILE_COUNT, 0.0f});
    glm_scale(gridTransform,
              (vec3){TILE_COUNT,
                     TILE_COUNT / ((float)winWidth / (float)winHeight), 1.0f});
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(gridTransform),
                    gridTransform[0]);

    glUseProgram(gridProgram);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // GUI

    glBindBuffer(GL_UNIFORM_BUFFER, uboUITransform);

    const float vw = (float)winWidth / (float)winHeight;
    const float vh = (float)winHeight / (float)winWidth;

    vec2 itemSize = {fminf(0.15f, 0.15f * vh), fminf(0.15f * vw, 0.15f)};
    vec2 gap = {fminf(0.01f, 0.01f * vh), fminf(0.01f * vw, 0.01f)};

    vec2 panelSize = {itemSize[0] * 5 + gap[0] * 6,
                      (itemSize[1] + gap[1] * 2) * 2};
    vec2 panelPos = {0.5f - panelSize[0] / 2.0f, 1.0f - panelSize[1] / 2.0f};

    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(vec2), panelPos);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(vec2), sizeof(vec2), panelSize);

    glBindBuffer(GL_UNIFORM_BUFFER, uboUIRect);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(vec4),
                    (vec4){0.f, 0.55f, 0.9f, 1.f});
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(vec4), sizeof(vec2),
                    (vec4){0.25f * panelSize[1] / panelSize[0] * vh, 0.25f});

    glUseProgram(rectProgram);
    glBindVertexArray(vao);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(vec4), sizeof(vec2),
                    (vec4){0.5f, 0.5f});

    for (int i = 0; i < sizeof(arrowGroups) / sizeof(*arrowGroups); i++) {
      for (int j = 0; j < 5; j++) {
        if (arrowGroups[i][j] == 0)
          continue;

        glBindBuffer(GL_UNIFORM_BUFFER, uboUIRect);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(vec4),
                        j == 0 ? (vec4){1.f, 0.96f, 0.8f, 1.f}
                               : (vec4){0.98f, 1.f, 1.f, 1.f});

        glBindBuffer(GL_UNIFORM_BUFFER, uboUITransform);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(vec2), sizeof(vec2),
                        itemSize);
        glBufferSubData(
            GL_UNIFORM_BUFFER, 0, sizeof(vec2),
            (vec2){panelPos[0] + gap[0] + (itemSize[0] + gap[0]) * j,
                   panelPos[1] + gap[1]});

        glUseProgram(rectProgram);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      }
    }

    // Game logic
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboPassID);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLint), &passId);
    passId = !passId;

    glUseProgram(arrowComputeProgram);
    glDispatchCompute(map.size, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    glfwSwapBuffers(window);
  }

  map_deinit(&map);

  glfwTerminate();

  return 0;
}