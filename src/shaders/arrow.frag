#version 430 core
#include "../arrows.h"

layout(location = 0) out vec4 FragColor;

layout(location = 1) in vec2 texCoord;

layout(location = 2) flat in Arrow arrow;

void main() {
  if (arrow.type == 0)
    discard;
  vec3 colors[] = {
      vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(1.0, 1.0, 0.0),
      vec3(0.8, 1.0, 0.2), vec3(0.0, 1.0, 1.0),
  };
  FragColor = vec4(colors[arrow.type % colors.length()], 1.0);
}