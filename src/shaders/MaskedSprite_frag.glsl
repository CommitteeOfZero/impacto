in vec2 uv;
in vec4 tint;

out vec4 color;

uniform sampler2D ColorMap;
uniform sampler2D Mask;
uniform vec2 Alpha;

void main() { 
	color = texture(ColorMap, uv);
	vec4 alp = texture(Mask, uv);
	alp.a *= Alpha.x;
	alp.a -= Alpha.y;
	alp.a = clamp(alp.a,0.0,1.0);
	color *= tint;
	color.a *= alp.a;
}