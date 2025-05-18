#version 430 core
#include "../arrows.h"

layout(location = 0) out vec4 FragColor;

layout(location = 1) in vec2 texCoord;

layout(location = 2) flat in Arrow arrow;

uniform sampler2D atlas;

const vec2 tileSize = vec2(1.0 / 16.0);

void main() {
  if (arrow.type == 0)
    discard;
  uint texIndex = arrow.type - 1;
  vec2 uv = texCoord * tileSize +
            tileSize * vec2(float(texIndex % 16), float(int(texIndex) / 16));
  FragColor = texture(atlas, uv);
}