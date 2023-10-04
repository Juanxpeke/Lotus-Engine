#version 460 core

in vec3 fragPosition;
// in vec3 fragNormal;

out vec4 fragColor;

void main()
{  
  vec3 greenColor = vec3(0.15f, 0.4f, 0.0f);
  vec3 result = greenColor + fragPosition;
  fragColor = vec4(result, 1.0);
}