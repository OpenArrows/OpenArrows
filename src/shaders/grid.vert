#version 430 core
layout(location = 0) in vec2 position;

layout(location = 1) out vec2 texCoord;

layout(std140, binding = 0) uniform Transform {
  mat4 view;
  mat4 projection;
  float scale;
};

void main() {
  vec2 uv = position;
  uv.y = 1.0 - uv.y;
  texCoord = (view * vec4(uv, 0.0, 1.0)).xy / scale;
  gl_Position = vec4(2.0 * position - 1.0, 0.0, 1.0);
}