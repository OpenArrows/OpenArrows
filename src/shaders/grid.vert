#version 430 core
layout(location = 0) in vec4 position;

layout(location = 1) out vec2 texCoord;

layout(std140, binding = 0) uniform Transform {
  mat4 model;
  mat4 view;
  mat4 projection;
};

void main() {
  texCoord = position.xy;
  gl_Position = 2.0 * position - 1.0;
}