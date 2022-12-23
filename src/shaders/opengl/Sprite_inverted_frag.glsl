in vec2 uv;
in vec4 tint;

out vec4 color;

uniform sampler2D ColorMap;

void main() {
  color = texture(ColorMap, uv);
  color.rgb = vec3(1.0) - color.rgb;
  color *= tint;
}