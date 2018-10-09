in vec2 uv;

out vec4 color;

#ifdef MSAA_MODE_MULTISAMPLE_TEXTURE
uniform sampler2DMS Framebuffer3D;

vec4 textureMS(sampler2DMS tex, vec2 texcoord) {
  ivec2 iTexcoord = ivec2(floor(WindowDimensions * RenderScale * texcoord));
  vec4 result = vec4(0.0);
  for (int i = 0; i < MultisampleCount; i++) {
    result += texelFetch(tex, iTexcoord, i);
  }
  return result / MultisampleCount;
}
#else
uniform sampler2D Framebuffer3D;
#endif

// const int MultisampleCount;
// const vec2 WindowDimensions;
// const float RenderScale;
// Knowing these (especially MultisampleCount) at compile time allows
// optimisations

void main() {
#ifdef MSAA_MODE_MULTISAMPLE_TEXTURE
  color = textureMS(Framebuffer3D, uv);
#else
  color = texture(Framebuffer3D, uv);
#endif
}