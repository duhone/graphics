#version 450

precision highp float;

layout(location = 0) out mediump vec4 Color;

const int MaxSprites = 256;

const vec2 Vertices[4] = vec2[4](vec2(0.0, 0.0), vec2(1.0, 0.0), vec2(0.0, 1.0), vec2(1.0, 1.0));

/*layout(set=0, binding = 0) uniform SpriteData{
  vec2 InvScreenSize;
  vec2 Size;
  // x,y are upper left corner, z and w are unused
  vec4 Offsets[MaxSprites];
  vec4 Colors[MaxSprites];
} data;*/

void main() {
  vec2 position = Vertices[gl_VertexIndex];

  //position *= data.Size;
  //position += data.Offsets[gl_InstanceIndex].xy;

  // from pixel coords to -1to1
 // highp vec2 pos = (position * vec2(2) * data.InvScreenSize) - vec2(1.0);
  //gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);
  gl_Position = vec4(position.x, position.y, 0.0, 1.0);

  //Color = data.Colors[gl_InstanceIndex];
  Color = vec4(0.0,1.0,0.0,1.0);
}