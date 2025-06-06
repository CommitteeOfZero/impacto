flat in uint colorMap;
in vec2 uv;
in vec4 tint;
flat in uint mask;
in vec2 maskUV;

out vec4 color;

uniform sampler2D[15] Textures;
uniform vec2 Alpha;
uniform bool IsInverted;

float rgbToLightness(vec3 rgb) {
    float maxVal = max(max(rgb.r, rgb.g), rgb.b);
    float minVal = min(min(rgb.r, rgb.g), rgb.b);
    
    return (maxVal + minVal) / 2.0;
}

void main() { 
	color = texture(Textures[colorMap], uv);
	vec4 alp = texture(Textures[mask], maskUV);
	float maskAlpha = rgbToLightness(alp.rgb);
	if (IsInverted) maskAlpha = 1.0f - maskAlpha;
	
	maskAlpha *= Alpha.x;
	maskAlpha -= Alpha.y;
	maskAlpha = clamp(maskAlpha,0.0,1.0);
	color *= tint;
	color.a *= maskAlpha;
	
}