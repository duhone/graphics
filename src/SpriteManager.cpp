#include "SpriteManager.h"

#include "Commands.h"
#include "SpriteTemplateImpl.h"
#include "SpriteTypeImpl.h"

#include "core/Log.h"

using namespace std;
using namespace CR;
using namespace CR::Core;
using namespace CR::Graphics;

SpriteManager::SpriteManager() {
	m_spriteTypes.Used.reset();
	m_spriteTemplates.Used.reset();
	m_sprites.Used.reset();
	m_sprites.UniformBuffer = UniformBufferDynamic{sizeof(SpriteUniformData) * MaxSprites};
}

SpriteManager::~SpriteManager() {
	if(m_spriteTypes.Used.any()) {
		Core::Log::Fail("not all sprite types were deleted when shutting down the graphics engine");
	}
}

uint8_t SpriteManager::CreateType(const std::string_view a_name, Pipeline&& a_pipeline) {
	size_t result = 0;
	for(size_t i = 0; i < m_spriteTypes.Used.size(); ++i) {
		if(m_spriteTypes.Used[i]) {
			result = i;
			break;
		}
	}
	if(result == m_spriteTypes.Used.size()) { Core::Log::Fail("Ran out of available sprite types"); }
	m_spriteTypes.Used[result]      = true;
	m_spriteTypes.Names[result]     = a_name;
	m_spriteTypes.Pipelines[result] = move(a_pipeline);

	return (uint8_t)result;
}

void SpriteManager::FreeType(uint8_t a_index) {
	m_spriteTypes.Pipelines[a_index] = Pipeline{};
	m_spriteTypes.Names[a_index].clear();
	m_spriteTypes.Names[a_index].shrink_to_fit();
	m_spriteTypes.Used[a_index] = false;
}

uint8_t SpriteManager::CreateTemplate(const std::string_view a_name, std::shared_ptr<SpriteType> a_type,
                                      const glm::uvec2& a_frameSize) {
	uint32_t typeIndex = ((SpriteTypeImpl*)a_type.get())->GetIndex();

	size_t result = 0;
	for(size_t i = typeIndex * SpriteTemplatesPerType; i < (typeIndex + 1) * SpriteTemplatesPerType; ++i) {
		if(!m_spriteTemplates.Used[i]) {
			result = i;
			break;
		}
	}
	if(result == (typeIndex + 1) * SpriteTemplatesPerType) { Core::Log::Fail("Ran out of available sprite templates"); }
	m_spriteTemplates.Used[result]        = true;
	m_spriteTemplates.Names[result]       = a_name;
	m_spriteTemplates.TypeIndices[result] = (uint8_t)typeIndex;
	m_spriteTemplates.Types[result]       = move(a_type);
	m_spriteTemplates.FrameSizes[result]  = a_frameSize;

	return (uint8_t)result;
}

void SpriteManager::FreeTemplate(uint8_t a_index) {
	m_spriteTemplates.Names[a_index].clear();
	m_spriteTemplates.Names[a_index].shrink_to_fit();
	m_spriteTemplates.Types[a_index].reset();
	m_spriteTemplates.Used[a_index] = false;
}

uint16_t SpriteManager::CreateSprite(const std::string_view a_name, std::shared_ptr<SpriteTemplate> a_template) {
	uint32_t templateIndex = ((SpriteTemplateImpl*)a_template.get())->GetIndex();

	uint32_t result = 0;
	for(uint32_t i = templateIndex * SpritesPerTemplate; i < (templateIndex + 1) * SpritesPerTemplate; ++i) {
		if(!m_sprites.Used[i]) {
			result = i;
			break;
		}
	}
	if(result == (templateIndex + 1) * SpritesPerTemplate) { Core::Log::Fail("Ran out of available sprites"); }
	m_sprites.Used[result]            = true;
	m_sprites.Names[result]           = a_name;
	m_sprites.TemplateIndices[result] = (uint8_t)templateIndex;
	m_sprites.Templates[result]       = move(a_template);

	return (uint16_t)result;
}

void SpriteManager::FreeSprite(uint16_t a_index) {
	m_sprites.Names[a_index].clear();
	m_sprites.Names[a_index].shrink_to_fit();
	m_sprites.Templates[a_index].reset();
	m_sprites.Used[a_index] = false;
}

void SpriteManager::Draw(CommandBuffer& a_commandBuffer) {
	SpriteUniformData* uniformData = m_sprites.UniformBuffer.GetData<SpriteUniformData>();
	for(uint32_t sprite = 0; sprite < MaxSprites; ++sprite) {
		if(m_sprites.Used[sprite]) {
			uniformData[sprite].Position.x = m_sprites.Positions[sprite].x;
			uniformData[sprite].Position.y = m_sprites.Positions[sprite].y;
			uniformData[sprite].Position.z = 1.0f;
			uniformData[sprite].Position.w = 1.0f;

			uniformData[sprite].Color = m_sprites.Colors[sprite];
		}
	}

	for(uint32_t type = 0; type < MaxSpriteTypes; ++type) {
		if(m_spriteTypes.Used[type]) {
			Core::Log::Assert(m_spriteTypes.Pipelines[type], "Sprite type didn't have a pipeline");
			Commands::BindPipeline(a_commandBuffer, m_spriteTypes.Pipelines[type]);
			for(uint32_t templ = type * SpriteTemplatesPerType; templ < (type + 1) * SpriteTemplatesPerType; ++templ) {
				if(m_spriteTemplates.Used[templ]) {
					uint32_t numSprites = 0;
					for(uint32_t sprite = templ * SpritesPerTemplate; sprite < (templ + 1 * SpritesPerTemplate);
					    ++sprite) {
						if(m_sprites.Used[sprite]) { ++numSprites; }
					}
					// shader will take framesize as float for perf reasons.
					glm::vec2 frameSize = m_spriteTemplates.FrameSizes[templ];
					Commands::PushConstants(a_commandBuffer, m_spriteTypes.Pipelines[type],
					                        Span<std::byte>{(byte*)&frameSize, sizeof(frameSize)});
					Commands::Draw(a_commandBuffer, 4, numSprites);
				}
			}
		}
	}
}
