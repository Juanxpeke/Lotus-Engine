#version 460 core

uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 position;
// in vec3 normal;

out vec3 fragPosition;
// out vec3 fragNormal;

void main() {
	gl_Position = projection * view * vec4(position, 1.0);
  fragPosition = position;
  // fragNormal = normal;  
}