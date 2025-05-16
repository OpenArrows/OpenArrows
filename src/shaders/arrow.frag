#version 430 core
#include "../arrows.h"

layout(location = 0) out vec4 FragColor;

layout(location = 1) in vec2 texCoord;

layout(std430, binding = 1) buffer Map { Chunk chunks[]; };

void main() { FragColor = vec4(texCoord / 10.0, 0.0, 1.0); }