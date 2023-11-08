#version 460 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in uint drawid;
layout (location = 3) in mat4 instanceMatrix;

out vec2 uv;
flat out uint drawID;

void main(void)
{
  uv = texCoord;
  drawID = drawid;
  gl_Position = instanceMatrix * vec4(position,0.0,1.0);
}