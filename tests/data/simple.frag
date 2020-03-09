#version 450

precision mediump float;
precision mediump int;

layout (constant_id = 0) const int c_maxTextures = 4096;

layout(location = 0) in vec4 Color;
layout(location = 1) in vec2 UV;
layout(location = 2) in flat int TextureIndex;

layout(binding = 1) uniform sampler2D textureSampler[c_maxTextures];

layout(location = 0) out vec4 fragColor;

void main()
{	
    vec4 tex = texture(textureSampler[TextureIndex], UV);
    fragColor = Color*tex;
}