#version 430 core
layout(location = 0) in vec2 position;

layout(location = 1) out vec2 texCoord;

layout(std140, binding = 0) uniform Transform { mat4 view; };

void main() {
  texCoord = position;
  texCoord.y = 1.0 - texCoord.y;
  vec4 pos = view * vec4(position, 0.0, 1.0);
  gl_Position = vec4(2.0 * pos.xy - 1.0, 0.0, 1.0);
}