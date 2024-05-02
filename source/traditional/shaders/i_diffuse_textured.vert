#version 460 core

// Shader storage buffer with the models of each mesh instance
layout(std140, binding = 2) readonly buffer Models
{
  // When using [], then the size of this array is determined at the time the shader
  // is executed. The size is the rest of this buffer object range
	mat4[] models;
};

layout(location = 5) uniform mat4 view;
layout(location = 6) uniform mat4 projection;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

flat out uint fragInstanceID;
out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTexCoord;

void main()
{
	int meshInstanceID = gl_InstanceID;

	mat4 model = models[meshInstanceID];

	fragInstanceID = meshInstanceID;
	fragPosition = vec3(model * vec4(position, 1.0));
	fragNormal = mat3(transpose(inverse(model))) * normal;
  fragTexCoord = texCoord;
	
	gl_Position = projection * view * model * vec4(position, 1.0);
}