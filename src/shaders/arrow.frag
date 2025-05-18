#version 430 core
#include "../arrows.h"

layout(location = 0) out vec4 FragColor;

layout(location = 1) in vec2 texCoord;

layout(location = 2) flat in Arrow arrow;
layout(location = 4) flat in ArrowState state;

uniform sampler2D atlas;

const vec2 tileSize = vec2(1.0 / 16.0);

const vec4 signalColors[] = {vec4(0.0, 0.0, 0.0, 0.0),
                             vec4(1.0, 0.0, 0.0, 0.8)};

void main() {
  if (arrow.type == 0)
    discard;
  uint texIndex = arrow.type - 1;
  vec2 uv = texCoord * tileSize +
            tileSize * vec2(float(texIndex % 16), float(int(texIndex) / 16));
  vec4 signalColor = signalColors[state.signal];
  vec4 texColor = texture(atlas, uv);
  FragColor = mix(signalColor, texColor, texColor.a);
}