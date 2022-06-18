in vec2 uv;
in vec4 tint;
in vec2 maskUV;

out vec4 oColor;

uniform sampler2D ColorMap;
uniform sampler2D Mask;
uniform vec4 Color;
uniform float AlphaRange;
uniform float ConstAlpha;
uniform float AlphaChk;

void main() {
	oColor = texture(ColorMap, uv);
	oColor.a += AlphaChk;
	oColor.a = clamp(oColor.a, 0.0, 1.0);
	vec4 aColor = texture(Mask, maskUV);
	aColor.a *= AlphaRange;
	aColor.a -= ConstAlpha;
	aColor.a = clamp(aColor.a, 0.0, 1.0);
	oColor.a *= aColor.a;
	oColor *= Color;
}