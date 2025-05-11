#version 430 core
#extension GL_OES_standard_derivatives : enable

layout(location = 0) out vec4 FragColor;

layout(location = 1) in vec2 texCoord;

float gridThickness = .06;

float filterWidth2(vec2 uv) {
  vec2 dx = dFdx(uv), dy = dFdy(uv);
  return dot(dx, dx) + dot(dy, dy) + .0001;
}

float gridSmooth(vec2 p) {
  vec2 q = p;
  q += .5;
  q -= floor(q);
  q = (gridThickness + 1.) * .5 - abs(q - .5);
  float w = 16. * filterWidth2(p);
  float s = sqrt(gridThickness);
  return smoothstep(.5 - w * s, .5 + w, max(q.x, q.y));
}

void main() { FragColor = vec4(0.0, 0.0, 0.0, 0.2) * gridSmooth(texCoord); }