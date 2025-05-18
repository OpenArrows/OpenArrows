#version 430 core
#include "../arrows.h"

layout(location = 0) in vec2 position;

layout(location = 1) out vec2 texCoord;

layout(location = 2) flat out Arrow arrow;
layout(location = 4) flat out ArrowState state;

layout(std140, binding = 0) uniform Transform {
  mat4 view;
  mat4 projection;
};

layout(std430, binding = 2) buffer Map { Chunk chunks[]; };

layout(std430, binding = 3) buffer MapState { ArrowState[256] states[]; };

void main() {
  int arrowIndex = gl_InstanceID % (CHUNK_SIZE * CHUNK_SIZE);
  int arrowX = arrowIndex % CHUNK_SIZE;
  int arrowY = arrowIndex / CHUNK_SIZE;
  int chunkIndex = gl_InstanceID - arrowIndex;
  uint chunkXy = chunks[chunkIndex].xy;
  int chunkX = int(bitfieldExtract(chunkXy, 0, 16));
  int chunkY = int(bitfieldExtract(chunkXy, 16, 16));
  texCoord = position;
  arrow = chunks[chunkIndex].arrows[arrowIndex];
  state = states[chunkIndex][arrowIndex];
  vec2 offset = vec2(float(chunkX * CHUNK_SIZE + arrowX),
                     float(chunkY * CHUNK_SIZE + arrowY));
  gl_Position = vec4((projection * view * vec4(offset + position, 0.0, 1.0)).xy,
                     0.0, 1.0);
}