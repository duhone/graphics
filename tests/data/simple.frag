#version 450

precision mediump float;

layout(location = 0) in vec4 Color;

layout(location = 0) out vec4 fragColor;

void main()
{			
  fragColor = Color;
}