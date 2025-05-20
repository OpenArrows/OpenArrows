#version 430 core
#extension GL_OES_standard_derivatives : enable

layout(location = 0) out vec4 FragColor;

layout(location = 1) in vec2 texCoord;

layout(std140, binding = 6) uniform UIRect {
  vec4 color;
  vec2 cornerRadius;
};

void main() {
  vec2 cxy1 = texCoord / cornerRadius / 2.0 - (0.5 / cornerRadius - 1.0) -
              vec2(0.25, 0.25);
  cxy1 = 2.0 * cxy1 - 1.0;
  vec2 cxy2 = texCoord / cornerRadius / 2.0 + vec2(0.25, 0.25);
  cxy2 = 2.0 * cxy2 - 1.0;
  vec2 cxy = max(abs(max(cxy1, 0.0)), abs(min(cxy2, 0.0)));
  float radius = dot(cxy, cxy) * 4.0;
  float delta = fwidth(radius);
  float alpha = 1.0 - smoothstep(1.0 - delta, 1.0 + delta, radius);
  FragColor = color;
  FragColor.a *= alpha;
}