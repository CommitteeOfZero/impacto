in vec2 uv;
in vec4 tint;
in vec2 maskUV;

out vec4 color;

uniform sampler2D ColorMap;
uniform sampler2D Mask;
uniform vec2 Alpha;
uniform bool IsInverted;
uniform bool IsSameTexture;

void main() { 
	color = texture(ColorMap, uv);
	vec4 alp = texture(Mask, maskUV);
	if (IsInverted) alp.a = 1.0f - alp.a;
	alp.a *= Alpha.x;
	alp.a -= Alpha.y;
	alp.a = clamp(alp.a,0.0,1.0);
	if (IsSameTexture) {
		color.a = alp.a;
		color.a *= tint.a;
	} else {
		color *= tint;
		color.a *= alp.a;
	}
}