#version 450 core

uniform sampler2D our_texture;

precision mediump float;

in vec2 texCoord;
in vec4 vertColor;
out vec4 fragColor;

void main()
{
  fragColor = texture(our_texture, texCoord) * vertColor;
}
