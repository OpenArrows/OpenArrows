#version 430 core
#include "../arrows.h"

layout(location = 0) in vec2 position;

layout(location = 1) out vec2 texCoord;

layout(std140, binding = 0) uniform Transform {
  mat4 view;
  mat4 projection;
};

layout(std430, binding = 2) buffer Map { Chunk chunks[]; };

void main() {
  int arrowIndex = gl_InstanceID % (CHUNK_SIZE * CHUNK_SIZE);
  int arrowX = arrowIndex % CHUNK_SIZE;
  int arrowY = arrowIndex / CHUNK_SIZE;
  int chunkIndex = gl_InstanceID - arrowIndex;
  texCoord = position;
  vec2 offset = vec2(float(arrowX), float(arrowY));
  gl_Position = vec4((projection * view * vec4(offset + position, 0.0, 1.0)).xy,
                     0.0, 1.0);
}