#version 450 core

layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 v_color;
layout(location = 2) in vec2 tcoord;

layout(location = 3) uniform mat4 model;
layout(location = 4) uniform vec4 u_color;
layout(location = 5) uniform bool u_color_en;

out vec4 vertColor;
out vec2 texCoord;

void main()
{
  gl_Position = model * pos;

  if(u_color_en)
    vertColor = u_color;
  else
    vertColor = v_color;

  texCoord = tcoord;
}
