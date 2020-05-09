#include "SpriteTemplateBasicImpl.h"

#include "EngineInternal.h"
#include "SpriteManagerBasic.h"

using namespace std;
using namespace CR;
using namespace CR::Graphics;

SpriteTemplateBasicImpl::SpriteTemplateBasicImpl(uint8_t a_index) : m_index(a_index) {}

SpriteTemplateBasicImpl::~SpriteTemplateBasicImpl() {
	GetSpriteManagerBasic().FreeTemplate(m_index);
}

std::shared_ptr<Graphics::SpriteTemplateBasic>
    Graphics::CreateSpriteTemplateBasic(const SpriteTemplateBasicCreateInfo& a_info) {
	uint8_t index = GetSpriteManagerBasic().CreateTemplate(a_info.Name, a_info.FrameSize, a_info.FrameRate,
	                                                       a_info.TextureName.c_str());
	return make_shared<Graphics::SpriteTemplateBasicImpl>(index);
}
