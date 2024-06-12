#version 460 core

layout(location = 0) uniform mat4 mvp;
layout(location = 1) uniform mat4 model;
layout(location = 2) uniform mat4 modelInverseTranspose;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTexCoord;

void main()
{
	fragPosition = vec3(model * vec4(position, 1.0f));
	fragNormal = mat3(modelInverseTranspose) * normal;
	fragTexCoord = texCoord;
	
	gl_Position = mvp * vec4(position, 1.0f);
}