layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 UV;
layout(location = 3) in lowp ivec4 BoneIndices[4];  // indices into Mesh.BoneMap
layout(location = 4) in vec4 BoneWeights;

uniform mat4 ViewProjection;

void main() { gl_Position = ViewProjection * vec4(Position, 1.0); }