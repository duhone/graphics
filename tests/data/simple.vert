#version 450

precision highp float;

layout(location = 0) out mediump vec4 Color;

const int MaxSprites = 256; //max in one batch, i.e. sprites per template

const vec2 Vertices[4] = vec2[4](vec2(0.0, 0.0), vec2(1.0, 0.0), vec2(0.0, 1.0), vec2(1.0, 1.0));

layout (constant_id = 0) const float InvScreenSizeX = 1.0/1280.0;
layout (constant_id = 1) const float InvScreenSizeY = 1.0/720.0;

layout(std430, push_constant) uniform SpriteTemplateData
{
    vec2 FrameSize;
} templateData;

struct SpriteUniformData{
  // x,y are upper left corner, z and w are unused
  vec4 Offset;
  vec4 Color;
};

layout(binding = 0) uniform SpriteData{
  SpriteUniformData Data[MaxSprites];
} spriteData;

void main() {
  vec2 position = Vertices[gl_VertexIndex];

  position *= templateData.FrameSize;
  position += spriteData.Data[gl_InstanceIndex].Offset.xy;

  // from pixel coords to -1to1
  vec2 pos = (position * vec2(2.0) * vec2(InvScreenSizeX, InvScreenSizeY)) - vec2(1.0);
  gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);

  Color = spriteData.Data[gl_InstanceIndex].Color;
}