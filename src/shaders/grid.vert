#version 430 core
layout(location = 0) in vec4 position;

layout(location = 1) out vec2 texCoord;

layout(std140, binding = 0) uniform Transform { mat4 view; };

void main() {
  vec4 uv = position;
  uv.y = 1.0 - uv.y;
  texCoord = (view * uv).xy;
  gl_Position = 2.0 * position - 1.0;
}