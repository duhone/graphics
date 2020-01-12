#version 450

precision highp float;

layout(location = 0) out mediump vec4 Color;

const int MaxSprites = 256;

const vec2 Vertices[4] = vec2[4](vec2(0.0, 0.0), vec2(1.0, 0.0), vec2(0.0, 1.0), vec2(1.0, 1.0));

layout (constant_id = 0) const float InvScreenSizeX = 1.0/1280.0;
layout (constant_id = 1) const float InvScreenSizeY = 1.0/720.0;

layout(push_constant) uniform SpriteTemplateData
{
    vec2 Size;
} templateData;

/*layout(set=0, binding = 0) uniform SpriteData{
  // x,y are upper left corner, z and w are unused
  vec4 Offsets[MaxSprites];
  vec4 Colors[MaxSprites];
} data;*/

void main() {
  vec2 position = Vertices[gl_VertexIndex];

  position *= templateData.Size;
  //position += data.Offsets[gl_InstanceIndex].xy;

  // from pixel coords to -1to1
  vec2 pos = (position * vec2(2.0) * vec2(InvScreenSizeX, InvScreenSizeY)) - vec2(1.0);
  gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);

  //Color = data.Colors[gl_InstanceIndex];
  Color = vec4(0.0,1.0,0.0,1.0);
}