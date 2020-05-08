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

uint8_t SpriteManagerBasic::CreateTemplate(const std::string_view a_name, const glm::uvec2& a_frameSize) {
	size_t result = MaxSpriteTemplates;
	for(size_t i = 0; i < MaxSpriteTemplates; ++i) {
		if(!m_spriteTemplates.Used[i]) {
			result = i;
			break;
		}
	}
	Core::Log::Require(result != MaxSpriteTemplates, "Ran out of available sprite templates");

	m_spriteTemplates.Used[result]       = true;
	m_spriteTemplates.Names[result]      = a_name;
	m_spriteTemplates.FrameSizes[result] = a_frameSize;

	return (uint8_t)result;
}

void SpriteManagerBasic::FreeTemplate(uint8_t a_index) {
	m_spriteTemplates.Names[a_index].clear();
	m_spriteTemplates.Names[a_index].shrink_to_fit();
	m_spriteTemplates.Used[a_index] = false;
}

uint16_t SpriteManagerBasic::CreateSprite(const std::string_view a_name,
                                          std::shared_ptr<SpriteTemplateBasic> a_template, const char* a_textureName) {
	uint32_t templateIndex = ((SpriteTemplateBasicImpl*)a_template.get())->GetIndex();

	uint32_t result = 0;
	for(uint32_t i = templateIndex * SpritesPerTemplate; i < (templateIndex + 1) * SpritesPerTemplate; ++i) {
		if(!m_sprites.Used[i]) {
			result = i;
			break;
		}
	}
	Core::Log::Assert(result != (templateIndex + 1) * SpritesPerTemplate, "Ran out of available sprites");

	m_sprites.Used[result]            = true;
	m_sprites.Names[result]           = a_name;
	m_sprites.TemplateIndices[result] = (uint8_t)templateIndex;
	m_sprites.Templates[result]       = move(a_template);
	m_sprites.TextureIndices[result]  = TextureSets::GetTextureIndex(a_textureName);
	m_sprites.MaxFrames[result]       = TextureSets::GetMaxFrames(m_sprites.TextureIndices[result]);
	m_sprites.FrameRates[result]      = SpriteBasic::eFrameRate::None;
	m_sprites.CurrentFrame[result]    = 0;

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
			switch(m_sprites.FrameRates[sprite]) {
				case SpriteBasic::eFrameRate::None:
					// Nothing to do
					break;
				case SpriteBasic::eFrameRate::FPS10:
					if(m_currentFrame % 6 == 0) {
						++m_sprites.CurrentFrame[sprite];
						m_sprites.CurrentFrame[sprite] %= m_sprites.MaxFrames[sprite];
					}
					break;
				case SpriteBasic::eFrameRate::FPS12:
					if(m_currentFrame % 5 == 0) {
						++m_sprites.CurrentFrame[sprite];
						m_sprites.CurrentFrame[sprite] %= m_sprites.MaxFrames[sprite];
					}
					break;
				case SpriteBasic::eFrameRate::FPS15:
					if(m_currentFrame % 4 == 0) {
						++m_sprites.CurrentFrame[sprite];
						m_sprites.CurrentFrame[sprite] %= m_sprites.MaxFrames[sprite];
					}
					break;
				case SpriteBasic::eFrameRate::FPS20:
					if(m_currentFrame % 3 == 0) {
						++m_sprites.CurrentFrame[sprite];
						m_sprites.CurrentFrame[sprite] %= m_sprites.MaxFrames[sprite];
					}
					break;
				case SpriteBasic::eFrameRate::FPS30:
					if(m_currentFrame % 2 == 0) {
						++m_sprites.CurrentFrame[sprite];
						m_sprites.CurrentFrame[sprite] %= m_sprites.MaxFrames[sprite];
					}
					break;
				case SpriteBasic::eFrameRate::FPS60: {
					++m_sprites.CurrentFrame[sprite];
					m_sprites.CurrentFrame[sprite] %= m_sprites.MaxFrames[sprite];
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
			uniformData[sprite].Position.x = m_sprites.Positions[sprite].x;
			uniformData[sprite].Position.y = m_sprites.Positions[sprite].y;
			uniformData[sprite].Position.z = m_sprites.TextureIndices[sprite];
			uniformData[sprite].Position.w = m_sprites.CurrentFrame[sprite];

			uniformData[sprite].Color = m_sprites.Colors[sprite];
		}
	}

	Core::Log::Assert(Pipeline, "Sprite type didn't have a pipeline");
	Commands::BindPipeline(a_commandBuffer, Pipeline);
	uint32_t descOffset = 0;
	for(uint32_t templ = 0; templ < MaxSpriteTemplates; ++templ) {
		if(m_spriteTemplates.Used[templ]) {
			uint32_t numSprites = 0;
			for(uint32_t sprite = templ * SpritesPerTemplate; sprite < (templ + 1 * SpritesPerTemplate); ++sprite) {
				if(m_sprites.Used[sprite]) { ++numSprites; }
			}
			// shader will take framesize as float for perf reasons.
			glm::vec2 frameSize = m_spriteTemplates.FrameSizes[templ];
			Commands::PushConstants(a_commandBuffer, Pipeline, Span<std::byte>{(byte*)&frameSize, sizeof(frameSize)});
			descOffset = templ * SpritesPerTemplate * sizeof(SpriteUniformData);
			Commands::BindDescriptorSet(a_commandBuffer, Pipeline, DescSet, descOffset);
			Commands::Draw(a_commandBuffer, 4, numSprites);
		}
	}
}
