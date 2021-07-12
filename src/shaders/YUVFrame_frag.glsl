in vec2 uv;
in vec4 tint;

out vec4 color;

uniform sampler2D Luma;
uniform sampler2D Cb;
uniform sampler2D Cr;
uniform bool IsAlpha;

const mat4 yuv_to_rgb_rec601 = mat4(
    1.16438,  0.00000,  1.59603, -0.87079,
    1.16438, -0.39176, -0.81297,  0.52959,
    1.16438,  2.01723,  0.00000, -1.08139,
    0, 0, 0, 1
);

vec4 getRGBA(vec2 texUv) {
    vec4 yuvcolor = vec4(1.0);
    yuvcolor.r = texture(Luma, texUv).r;
    yuvcolor.g = texture(Cb, texUv).r;
    yuvcolor.b = texture(Cr, texUv).r;
    return yuvcolor * yuv_to_rgb_rec601;
}

void main() {
    if (IsAlpha) {
        color = getRGBA(vec2(uv.x, uv.y / 2.0));
        float alpha = getRGBA(vec2(uv.x, uv.y / 2.0 + 0.5)).r;
        color.a = alpha >= 0.9 ? 1.0 : alpha;
    } else {
        color = getRGBA(uv);
    }
	color *= tint;
}