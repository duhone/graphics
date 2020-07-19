#include "SpriteManagerBasic.h"

#include "Commands.h"
#include "Constants.h"
#include "SpriteTemplateBasicImpl.h"
#include "shaders/Basic.h"

#include "core/Log.h"

using namespace std;
using namespace CR;
using namespace CR::Core;
using namespace CR::Graphics;

SpriteManagerBasic::SpriteManagerBasic() {
	m_spriteTemplates.Used.reset();
	m_sprites.Used.reset();

	Vertex dummy;
	VertexBufferLayout layout;
	layout.AddVariable(dummy.Offset);
	layout.AddVariable(dummy.TextureFrame);
	layout.AddVariable(dummy.Color);
	layout.AddVariable(dummy.FrameSize);
	layout.AddVariable(dummy.Rotation);
	m_vertexBuffer = VertexBuffer<Vertex>(layout, c_maxSprites);

	CreatePipelineArgs pipeInfo;
	pipeInfo.ShaderModule      = embed::GetBasic();
	pipeInfo.BindingDesc       = m_vertexBuffer.GetBindingDescription();
	pipeInfo.AttribDescription = m_vertexBuffer.GetAttrDescriptions();
	Pipeline                   = Graphics::Pipeline(pipeInfo);

	DescSet = CreateDescriptorSet(Pipeline.GetDescLayout());
}

SpriteManagerBasic::~SpriteManagerBasic() {
	Core::Log::Assert(!m_spriteTemplates.Used.any(),
	                  "not all sprite types were deleted when shutting down the graphics engine");
}

uint8_t SpriteManagerBasic::CreateTemplate(const std::string_view a_name, const glm::uvec2& a_frameSize,
                                           eFrameRate frameRate, const char* a_textureName) {
	size_t result = c_maxSpriteTemplates;
	for(size_t i = 0; i < c_maxSpriteTemplates; ++i) {
		if(!m_spriteTemplates.Used[i]) {
			result = i;
			break;
		}
	}
	Core::Log::Require(result != c_maxSpriteTemplates, "Ran out of available sprite templates");

	m_spriteTemplates.Used[result]           = true;
	m_spriteTemplates.Ready[result]          = false;
	m_spriteTemplates.Names[result]          = a_name;
	m_spriteTemplates.FrameSizes[result]     = a_frameSize;
	m_spriteTemplates.TextureIndices[result] = TextureSets::GetTextureIndex(a_textureName);
	m_spriteTemplates.MaxFrames[result]      = TextureSets::GetMaxFrames(m_spriteTemplates.TextureIndices[result]);
	m_spriteTemplates.FrameRates[result]     = frameRate;

	return (uint8_t)result;
}

void SpriteManagerBasic::FreeTemplate(uint8_t a_index) {
	m_spriteTemplates.Names[a_index].clear();
	m_spriteTemplates.Names[a_index].shrink_to_fit();
	m_spriteTemplates.Used[a_index] = false;
}

uint16_t SpriteManagerBasic::CreateSprite(const std::string_view a_name,
                                          std::shared_ptr<SpriteTemplateBasic> a_template) {
	uint32_t templateIndex = ((SpriteTemplateBasicImpl*)a_template.get())->GetIndex();

	uint32_t result = c_maxSprites;
	for(uint32_t i = 0; i < c_maxSprites; ++i) {
		if(!m_sprites.Used[i]) {
			result = i;
			break;
		}
	}
	Core::Log::Assert(result != c_maxSprites, "Ran out of available sprites");

	m_sprites.Used[result]            = true;
	m_sprites.Names[result]           = a_name;
	m_sprites.TemplateIndices[result] = (uint8_t)templateIndex;
	m_sprites.Templates[result]       = move(a_template);
	m_sprites.CurrentFrame[result]    = 0;
	m_sprites.Colors[result]          = glm::vec4(1.0f);
	m_sprites.Positions[result]       = glm::vec2(0.0f);
	m_sprites.Rotations[result]       = 0.0f;

	return (uint16_t)result;
}

void SpriteManagerBasic::FreeSprite(uint16_t a_index) {
	m_sprites.Names[a_index].clear();
	m_sprites.Names[a_index].shrink_to_fit();
	m_sprites.Templates[a_index].reset();
	m_sprites.Used[a_index] = false;
}

void SpriteManagerBasic::Frame(CommandBuffer& a_commandBuffer) {
	++m_currentFrame;

	for(size_t i = 0; i < c_maxSpriteTemplates; ++i) {
		if(m_spriteTemplates.Used[i] && !m_spriteTemplates.Ready[i]) {
			m_spriteTemplates.Ready[i] = TextureSets::IsReady(m_spriteTemplates.TextureIndices[i]);
		}
	}

	Pipeline.Frame(DescSet);

	Vertex* spriteData    = m_vertexBuffer.begin();
	m_numSpritesThisFrame = 0;

	for(uint32_t sprite = 0; sprite < c_maxSprites; ++sprite) {
		if(!m_sprites.Used[sprite]) { continue; }
		auto& templIndex = m_sprites.TemplateIndices[sprite];
		if(!m_spriteTemplates.Ready[templIndex]) { continue; }

		switch(m_spriteTemplates.FrameRates[templIndex]) {
			case eFrameRate::None:
				// Nothing to do
				break;
			case eFrameRate::FPS10:
				if(m_currentFrame % (6 * GetFrameRateDivisor()) == 0) {
					++m_sprites.CurrentFrame[sprite];
					m_sprites.CurrentFrame[sprite] %= m_spriteTemplates.MaxFrames[templIndex];
				}
				break;
			case eFrameRate::FPS12:
				if(m_currentFrame % (5 * GetFrameRateDivisor()) == 0) {
					++m_sprites.CurrentFrame[sprite];
					m_sprites.CurrentFrame[sprite] %= m_spriteTemplates.MaxFrames[templIndex];
				}
				break;
			case eFrameRate::FPS15:
				if(m_currentFrame % (4 * GetFrameRateDivisor()) == 0) {
					++m_sprites.CurrentFrame[sprite];
					m_sprites.CurrentFrame[sprite] %= m_spriteTemplates.MaxFrames[templIndex];
				}
				break;
			case eFrameRate::FPS20:
				if(m_currentFrame % (3 * GetFrameRateDivisor()) == 0) {
					++m_sprites.CurrentFrame[sprite];
					m_sprites.CurrentFrame[sprite] %= m_spriteTemplates.MaxFrames[templIndex];
				}
				break;
			case eFrameRate::FPS30:
				if(m_currentFrame % (2 * GetFrameRateDivisor()) == 0) {
					++m_sprites.CurrentFrame[sprite];
					m_sprites.CurrentFrame[sprite] %= m_spriteTemplates.MaxFrames[templIndex];
				}
				break;
			case eFrameRate::FPS60: {
				if(m_currentFrame % (1 * GetFrameRateDivisor()) == 0) {
					++m_sprites.CurrentFrame[sprite];
					m_sprites.CurrentFrame[sprite] %= m_spriteTemplates.MaxFrames[templIndex];
				}
			} break;
			default:
				break;
		}

		float sinAngle = sin(m_sprites.Rotations[sprite]);
		float cosAngle = cos(m_sprites.Rotations[sprite]);
		glm::mat2 rot  = glm::mat2{cosAngle, -sinAngle, sinAngle, cosAngle};

		spriteData->Offset       = m_sprites.Positions[sprite];
		spriteData->TextureFrame = {m_spriteTemplates.TextureIndices[templIndex], m_sprites.CurrentFrame[sprite]};
		spriteData->Color        = m_sprites.Colors[sprite];
		spriteData->FrameSize    = m_spriteTemplates.FrameSizes[templIndex];
		spriteData->Rotation     = glm::vec4{rot[0][0], rot[0][1], rot[1][0], rot[1][1]};

		++spriteData;
		++m_numSpritesThisFrame;
	}
	m_vertexBuffer.Release(a_commandBuffer);
}

void SpriteManagerBasic::Draw(CommandBuffer& a_commandBuffer) {
	Core::Log::Assert(Pipeline, "Sprite type didn't have a pipeline");

	m_vertexBuffer.Acquire(a_commandBuffer);
	if(m_numSpritesThisFrame > 0) {
		Commands::BindPipeline(a_commandBuffer, Pipeline);
		Commands::BindVertexBuffer(a_commandBuffer, m_vertexBuffer.GetHandle());
		Commands::BindDescriptorSet(a_commandBuffer, Pipeline, DescSet);
		Commands::Draw(a_commandBuffer, 4, m_numSpritesThisFrame);
	}
}
