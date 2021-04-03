in vec2 uv;
in vec4 tint;

out vec4 color;

uniform sampler2D Luma;
uniform sampler2D Cb;
uniform sampler2D Cr;

const mat4 yuv_to_rgb_rec601 = mat4(
    1.16438,  0.00000,  1.59603, -0.87079,
    1.16438, -0.39176, -0.81297,  0.52959,
    1.16438,  2.01723,  0.00000, -1.08139,
    0, 0, 0, 1
);

void main() {
    vec4 yuvcolor = vec4(1.0);
    yuvcolor.r = texture(Luma, uv).r;
    yuvcolor.g = texture(Cb, uv).r;
    yuvcolor.b = texture(Cr, uv).r;
    color = yuvcolor * yuv_to_rgb_rec601;
	color *= tint;
}