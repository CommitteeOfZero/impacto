flat in uint colorMap;
in vec2 uv;
in vec4 tint;
flat in uint mask;
in vec2 maskUV;

out vec4 color;

uniform sampler2D[15] Textures;
uniform vec2 Alpha;
uniform bool IsInverted;
uniform bool IsSameTexture;

void main() { 
	color = texture(Textures[colorMap], uv);
	vec4 alp = texture(Textures[mask], maskUV);
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