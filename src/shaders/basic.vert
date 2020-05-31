#version 450

precision highp float;

layout(location = 0) out mediump vec4 Color;
layout(location = 1) out mediump vec3 UV;
layout(location = 2) out flat mediump int TextureIndex;

const vec2 Vertices[4] = vec2[4](vec2(0.0, 0.0), vec2(1.0, 0.0), vec2(0.0, 1.0), vec2(1.0, 1.0));

layout (constant_id = 0) const float InvScreenSizeX = 1.0/1280.0;
layout (constant_id = 1) const float InvScreenSizeY = 1.0/720.0;
//max in one batch, keep uniform mapped range from getting too large
layout (constant_id = 2) const int MaxSprites = 256;

struct SpriteUniformData{
  // x,y are upper left corner, z is the texture to use, and w is the current frame
  vec4 Offset;
  vec4 Color;
  vec4 FrameSize; //size in x,y.  z,w unused
  vec4 Rotation; //really a mat2, packed
};

layout(binding = 0) uniform SpriteData{
  SpriteUniformData Data[MaxSprites];
} spriteData;

void main() {
  vec2 position = Vertices[gl_VertexIndex];
  UV.xy = position;
  UV.z = spriteData.Data[gl_InstanceIndex].Offset.w;
  
  vec4 rotVec = spriteData.Data[gl_InstanceIndex].Rotation;
  mat2 rot = mat2(rotVec.x, rotVec.y, rotVec.z, rotVec.w);
  position = rot * (position-0.5);
  position += 0.5;

  position *= spriteData.Data[gl_InstanceIndex].FrameSize.xy;
  position += spriteData.Data[gl_InstanceIndex].Offset.xy;

  // from pixel coords to -1to1
  vec2 pos = (position * vec2(2.0) * vec2(InvScreenSizeX, InvScreenSizeY)) - vec2(1.0);
  gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);

  Color = spriteData.Data[gl_InstanceIndex].Color;
  TextureIndex = int(spriteData.Data[gl_InstanceIndex].Offset.z);
}