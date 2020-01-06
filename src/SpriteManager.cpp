#include "SpriteManager.h"

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
	m_spriteTypes.Used[a_index] = false;
	m_spriteTypes.Names[a_index].clear();
	m_spriteTypes.Names[a_index].shrink_to_fit();
}
