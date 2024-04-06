#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout(location = 0) uniform mat4 mvpMatrix;
layout(location = 1) uniform mat4 modelMatrix;
layout(location = 2) uniform mat4 modelInverseTransposeMatrix;

out vec3 normal;
out vec3 worldPos;
out vec2 texCoord;

void main()
{
	normal = mat3(modelInverseTransposeMatrix) * aNormal;
	texCoord = aTexCoord;
	worldPos = vec3(modelMatrix * vec4(aPos,1.0f));
	gl_Position = mvpMatrix * vec4(aPos,1.0f);

}