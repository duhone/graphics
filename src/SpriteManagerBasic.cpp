#include "SpriteManagerBasic.h"

#include "Commands.h"
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

	CreatePipelineArgs pipeInfo;
	pipeInfo.ShaderModule = embed::GetBasic();
	Pipeline              = Graphics::Pipeline(pipeInfo);
	UniformBuffer         = UniformBufferDynamic{sizeof(SpriteUniformData) * MaxSprites};
	DescSet               = CreateDescriptorSet(Pipeline.GetDescLayout(), UniformBuffer);
}

SpriteManagerBasic::~SpriteManagerBasic() {
	Core::Log::Assert(!m_spriteTemplates.Used.any(),
	                  "not all sprite types were deleted when shutting down the graphics engine");
}

uint8_t SpriteManagerBasic::CreateTemplate(const std::string_view a_name, const glm::uvec2& a_frameSize,
                                           eFrameRate frameRate, const char* a_textureName) {
	size_t result = MaxSpriteTemplates;
	for(size_t i = 0; i < MaxSpriteTemplates; ++i) {
		if(!m_spriteTemplates.Used[i]) {
			result = i;
			break;
		}
	}
	Core::Log::Require(result != MaxSpriteTemplates, "Ran out of available sprite templates");

	m_spriteTemplates.Used[result]           = true;
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

	uint32_t result = MaxSprites;
	for(uint32_t i = 0; i < MaxSprites; ++i) {
		if(!m_sprites.Used[i]) {
			result = i;
			break;
		}
	}
	Core::Log::Assert(result != MaxSprites, "Ran out of available sprites");

	m_sprites.Used[result]            = true;
	m_sprites.Names[result]           = a_name;
	m_sprites.TemplateIndices[result] = (uint8_t)templateIndex;
	m_sprites.Templates[result]       = move(a_template);
	m_sprites.CurrentFrame[result]    = 0;
	m_sprites.Colors[result]          = glm::vec4(1.0f);
	m_sprites.Positions[result]       = glm::vec2(0.0f);

	return (uint16_t)result;
}

void SpriteManagerBasic::FreeSprite(uint16_t a_index) {
	m_sprites.Names[a_index].clear();
	m_sprites.Names[a_index].shrink_to_fit();
	m_sprites.Templates[a_index].reset();
	m_sprites.Used[a_index] = false;
}

void SpriteManagerBasic::Frame() {
	++m_currentFrame;
	Pipeline.Frame(DescSet);

	for(uint32_t sprite = 0; sprite < MaxSprites; ++sprite) {
		if(m_sprites.Used[sprite]) {
			auto& templIndex = m_sprites.TemplateIndices[sprite];
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
		}
	}
}

void SpriteManagerBasic::Draw(CommandBuffer& a_commandBuffer) {
	SpriteUniformData* uniformData = UniformBuffer.GetData<SpriteUniformData>();
	for(uint32_t sprite = 0; sprite < MaxSprites; ++sprite) {
		if(m_sprites.Used[sprite]) {
			auto& templIndex = m_sprites.TemplateIndices[sprite];

			uniformData[sprite].Position.x = m_sprites.Positions[sprite].x;
			uniformData[sprite].Position.y = m_sprites.Positions[sprite].y;
			uniformData[sprite].Position.z = m_spriteTemplates.TextureIndices[templIndex];
			uniformData[sprite].Position.w = m_sprites.CurrentFrame[sprite];

			uniformData[sprite].Color     = m_sprites.Colors[sprite];
			uniformData[sprite].FrameSize = glm::vec4(m_spriteTemplates.FrameSizes[templIndex], 0.0f, 0.0f);
		}
	}

	Core::Log::Assert(Pipeline, "Sprite type didn't have a pipeline");
	Commands::BindPipeline(a_commandBuffer, Pipeline);
	uint32_t descOffset = 0;
	uint32_t numSprites = 0;
	Commands::BindDescriptorSet(a_commandBuffer, Pipeline, DescSet, descOffset);
	for(uint32_t sprite = 0; sprite < MaxSprites; ++sprite) {
		if(m_sprites.Used[sprite]) {
			++numSprites;
			if(numSprites > 256) {
				Commands::Draw(a_commandBuffer, 4, numSprites);
				// new batch
				descOffset = MaxSpritesPerBatch * sizeof(SpriteUniformData);
				Commands::BindDescriptorSet(a_commandBuffer, Pipeline, DescSet, descOffset);
			}
		}
	}
	if(numSprites > 0) { Commands::Draw(a_commandBuffer, 4, numSprites); }
}
