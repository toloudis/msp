#version 410

// uniform inputs
uniform mat4 ModelviewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 NormalMatrix; // inverse transpose of modelview.

// per-vertex inputs
layout(location = 0) in vec4 InPosition;
layout(location = 1) in vec3 InNormal;
layout(location = 2) in vec2 InUV;
//in vec3 InBinormal;
//in vec3 InBitangent;

// outputs
smooth out vec4 CameraPos;
smooth out vec3 CameraNormal;
smooth out vec2 UV;
out gl_PerVertex
{
    vec4 gl_Position;
};
void main(void) {
    CameraPos = ModelviewMatrix * InPosition;
    CameraNormal = mat3(NormalMatrix) * InNormal;
	UV = InUV;
    gl_Position = ProjectionMatrix * CameraPos;
}
