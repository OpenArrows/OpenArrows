#version 430 core
layout(location = 0) in vec2 position;

layout(location = 1) out vec2 texCoord;

layout(std140, binding = 5) uniform UITransform {
  vec2 offset;
  vec2 size;
};

void main() {
  texCoord = position;
  vec2 pos = 2.0 * (offset + position * size) - 1.0;
  gl_Position = vec4(pos.x, -pos.y, 0.0, 1.0);
}