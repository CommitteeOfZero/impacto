in vec2 uv;
in vec4 tint;
in vec2 maskUV;

out vec4 color;

uniform sampler2D ColorMap;
uniform sampler2D Mask;
uniform vec2 Alpha;
uniform bool IsInverted;
uniform bool IsSameTexture;
uniform bool UseMaskAlpha;

void main() { 
	color = texture(ColorMap, uv);
	vec4 alp = texture(Mask, maskUV);
	float maskAlpha = (alp.r + alp.g + alp.b) / 3.0;
	if (IsInverted) maskAlpha = 1.0f - maskAlpha;
	
	maskAlpha *= Alpha.x;
	maskAlpha -= Alpha.y;
	maskAlpha = clamp(maskAlpha,0.0,1.0);
	if (IsSameTexture) {
		color.a = maskAlpha;
		color.a *= tint.a;
	} else {
		color *= tint;
		color.a *= maskAlpha;
	}
}