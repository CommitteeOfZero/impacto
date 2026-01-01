in vec2 uv;
in vec4 tint;
in vec2 maskUV;

out vec4 color;

uniform sampler2D ColorMap;
uniform sampler2D Mask;
uniform bool IsInverted;

void main() { 
	color = texture(ColorMap, uv);
    color.a *= texture(Mask, maskUV).r;
    color *= tint;
}
