#version 450 core

precision mediump float;

in vec4 vertColor;
out vec4 fragColor;

void main()
{
  fragColor = vertColor;
}
