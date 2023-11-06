#version 460

uniform sampler2D img;

in vec2 fragUV;

out vec4 outColor;

void main ()
{
  outColor = texture(img, fragUV);
}