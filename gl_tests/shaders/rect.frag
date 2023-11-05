#version 460 core

layout (location = 0) in vec2 uv;
layout (location = 1) flat in uint drawID;

layout (location = 0) out vec4 color;
layout (binding = 0) uniform sampler2DArray textureArray;

void main(void)
{
  color = texture(textureArray, vec3(uv.x, uv.y, drawID));
}