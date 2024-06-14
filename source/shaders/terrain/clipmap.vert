#version 460 core

layout(location = 0) uniform mat4 view;
layout(location = 1) uniform mat4 projection;

layout(location = 2) uniform float scale;
layout(location = 3) uniform vec2 offset;

layout(location = 0) in vec3 position;

void main()
{
/*
     38 	vec2 xy = offset + ( M * vec4( position, 1.0 ) ).xy * scale;
     39 	vec2 uv = ( xy + 0.5 ) / textureSize( heightmap, 0 );
     40 	vec2 height_sample = texelFetch( heightmap, ivec2( xy ), 0 ).rg;
     41 	float z = 256.0 * height_sample.r + height_sample.g;
     42 
     43 	v2f.view_position = V * vec4( xy, z, 1.0 );
     44 	v2f.world_position = vec3( xy, z );
     45 	v2f.uv = uv;
     46 	gl_Position = P * v2f.view_position;
*/
  vec2 xz = offset + vec4(position, 1.0).xz * scale;

  float y = 0.0;

  vec3 worldPosition = vec3(y);
  worldPosition.xz = xz;

	gl_Position = projection * view * vec4(worldPosition, 1.0);
}