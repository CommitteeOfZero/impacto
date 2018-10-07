in vec2 uv;

out vec4 color;

uniform sampler2D Framebuffer3D;
uniform sampler2DMS Framebuffer3DMS;

// const int MultisampleCount;
// const vec2 WindowDimensions;
// Knowing these (especially MultisampleCount) at compile time allows
// optimisations like compressed multisampling storage on Intel

vec4 textureMS(sampler2DMS tex, vec2 texcoord) {
  ivec2 iTexcoord = ivec2(floor(WindowDimensions * texcoord));
  vec4 result = vec4(0.0);
  for (int i = 0; i < MultisampleCount; i++) {
    result += texelFetch(tex, iTexcoord, i);
  }
  return result / MultisampleCount;
}

void main() {
  if (MultisampleCount > 0) {
    color = textureMS(Framebuffer3DMS, uv);
  } else {
    color = texture(Framebuffer3D, uv);
  }
}