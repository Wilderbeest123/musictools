#version 450 core

layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 color;
layout(location = 2) uniform mat4 model;

out vec4 vertColor;

void main()
{
  gl_Position = model * pos;
  vertColor = color;
}
