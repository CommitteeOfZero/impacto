flat in uint colorMap;
in vec2 uv;
in vec4 tint;
flat in uint mask;
in vec2 maskUV;

out vec4 color;

uniform sampler2D[15] Textures;
uniform vec4 Alpha;

void main() { 
	color = texture(Textures[colorMap], uv);
	color *= tint;
	vec4 alp = texture(Textures[mask], maskUV);
	float ga = alp.r;
	float ea = 1.0 - alp.g;
	float fa = 1.0 - alp.b;
	fa *= Alpha.r;
	fa -= Alpha.g;
	fa = clamp(fa,0.0,1.0);
	color.a *= fa;

	ea += Alpha.b;
	if (ea > 1.0) ea = 2.0 - ea;
	ea *= 2.0;
	if (ea > 1.0) ea = 2.0 - ea;

	ga *= ea;
	ga = clamp(ga, 0.0, 1.0);
	color.a -= ga;
	color.a = clamp(color.a,0.0,1.0);
}