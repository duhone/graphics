#include "SpriteTemplateImpl.h"

#include "SpriteManager.h"
#include "vulkan/EngineInternal.h"

using namespace std;
using namespace CR;
using namespace CR::Graphics;

SpriteTemplateImpl::SpriteTemplateImpl(uint8_t a_index) : m_index(a_index) {}

SpriteTemplateImpl::~SpriteTemplateImpl() {
	GetSpriteManager().FreeTemplate(m_index);
}

std::shared_ptr<Graphics::SpriteTemplate> Graphics::CreateSpriteTemplate(const SpriteTemplateCreateInfo& a_info) {
	uint8_t index = GetSpriteManager().CreateTemplate(a_info.Name, a_info.Type, a_info.FrameSize);
	return make_shared<Graphics::SpriteTemplateImpl>(index);
}
