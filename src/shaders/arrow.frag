#version 430 core
#include "../arrows.h"

layout(location = 0) out vec4 FragColor;

layout(location = 1) in vec2 texCoord;

layout(location = 2) flat in Arrow arrow;

uniform sampler2D atlas;

void main() {
  if (arrow.type == 0)
    discard;
  FragColor = texture(atlas, texCoord);
}