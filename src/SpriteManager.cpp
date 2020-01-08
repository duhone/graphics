#include "SpriteManager.h"

#include "Commands.h"
#include "SpriteTemplateImpl.h"
#include "SpriteTypeImpl.h"

#include "core/Log.h"

using namespace CR;
using namespace CR::Graphics;

SpriteManager::SpriteManager() {
	m_spriteTypes.Used.reset();
}

SpriteManager::~SpriteManager() {
	if(m_spriteTypes.Used.any()) {
		Core::Log::Fail("not all sprite types were deleted when shutting down the graphics engine");
	}
}

uint8_t SpriteManager::CreateType(const std::string_view a_name, std::unique_ptr<Pipeline> a_pipeline) {
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
	m_spriteTypes.Pipelines[a_index].reset();
	m_spriteTypes.Names[a_index].clear();
	m_spriteTypes.Names[a_index].shrink_to_fit();
	m_spriteTypes.Used[a_index] = false;
}

uint8_t SpriteManager::CreateTemplate(const std::string_view a_name, std::shared_ptr<SpriteType> a_type) {
	size_t result = 0;
	for(size_t i = 0; i < m_spriteTemplates.Used.size(); ++i) {
		if(m_spriteTemplates.Used[i]) {
			result = i;
			break;
		}
	}
	if(result == m_spriteTemplates.Used.size()) { Core::Log::Fail("Ran out of available sprite templates"); }
	m_spriteTemplates.Used[result]        = true;
	m_spriteTemplates.Names[result]       = a_name;
	m_spriteTemplates.TypeIndices[result] = ((SpriteTypeImpl*)a_type.get())->GetIndex();
	m_spriteTemplates.Types[result]       = move(a_type);

	return (uint8_t)result;
}

void SpriteManager::FreeTemplate(uint8_t a_index) {
	m_spriteTemplates.Names[a_index].clear();
	m_spriteTemplates.Names[a_index].shrink_to_fit();
	m_spriteTemplates.Types[a_index].reset();
	m_spriteTemplates.Used[a_index] = false;
}

uint16_t SpriteManager::CreateSprite(const std::string_view a_name, std::shared_ptr<SpriteTemplate> a_template) {
	size_t result = 0;
	for(size_t i = 0; i < m_sprites.Used.size(); ++i) {
		if(m_sprites.Used[i]) {
			result = i;
			break;
		}
	}
	if(result == m_sprites.Used.size()) { Core::Log::Fail("Ran out of available sprites"); }
	m_sprites.Used[result]            = true;
	m_sprites.Names[result]           = a_name;
	m_sprites.TemplateIndices[result] = ((SpriteTemplateImpl*)a_template.get())->GetIndex();
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
	for(uint8_t type = 0; type < MaxSpriteTypes; ++type) {
		if(m_spriteTypes.Used[type]) {
			Core::Log::Assert(m_spriteTypes.Pipelines[type].get(), "Sprite type didn't have a pipeline");
			Commands::BindPipeline(a_commandBuffer, *m_spriteTypes.Pipelines[type].get());
			for(uint8_t templ = 0; templ < MaxSpriteTemplates; ++templ) {
				if(m_spriteTemplates.Used[templ] && m_spriteTemplates.TypeIndices[templ] == type) {
					uint32_t numSprites = 0;
					for(uint16_t sprite = 0; sprite < MaxSprites; ++sprite) {
						if(m_sprites.Used[sprite] && m_sprites.TemplateIndices[sprite] == templ) { ++numSprites; }
					}
					Commands::Draw(a_commandBuffer, 4, numSprites);
				}
			}
		}
	}
}
