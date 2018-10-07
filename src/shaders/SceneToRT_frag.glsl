in vec2 uv;

out vec4 color;

uniform sampler2D Framebuffer3D;
uniform sampler2DMS Framebuffer3DMS;

uniform int MultisampleCount;

vec4 textureMS(sampler2DMS tex, vec2 texcoord) {
  ivec2 iTexcoord = ivec2(floor(textureSize(tex) * texcoord));
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