#version 450 core

layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 v_color;
layout(location = 2) uniform mat4 model;
layout(location = 3) uniform vec4 u_color;
layout(location = 4) uniform bool u_color_en;

out vec4 vertColor;

void main()
{
  gl_Position = model * pos;

  if(u_color_en)
    vertColor = u_color;
  else
    vertColor = v_color;
}
