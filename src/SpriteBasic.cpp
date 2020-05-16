#include "Graphics/SpriteBasic.h"

#include "EngineInternal.h"
#include "SpriteManagerBasic.h"

using namespace std;
using namespace CR;
using namespace CR::Graphics;

SpriteBasic::SpriteBasic(const SpriteBasicCreateInfo& a_info) {
	m_index = GetSpriteManagerBasic().CreateSprite(a_info.Name, a_info.Template);
}

SpriteBasic::~SpriteBasic() {
	if(m_index != c_unused) { GetSpriteManagerBasic().FreeSprite(m_index); }
}

SpriteBasic::SpriteBasic(SpriteBasic&& a_other) noexcept {
	*this = std::move(a_other);
}

SpriteBasic& SpriteBasic::operator=(SpriteBasic&& a_other) noexcept {
	if(m_index != c_unused) { GetSpriteManagerBasic().FreeSprite(m_index); }
	m_index         = a_other.m_index;
	a_other.m_index = c_unused;

	return *this;
}

void SpriteBasic::SetPosition(const glm::vec2& a_position) {
	GetSpriteManagerBasic().SetSpritePosition(m_index, a_position);
}

void SpriteBasic::SetColor(const glm::vec4& a_color) {
	GetSpriteManagerBasic().SetSpriteColor(m_index, a_color);
}
