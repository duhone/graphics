#include "SpriteTemplateImpl.h"

using namespace std;
using namespace CR;
using namespace CR::Graphics;

SpriteTemplateImpl::SpriteTemplateImpl(const SpriteTemplateCreateInfo& a_info) :
    m_name(a_info.Name), m_type(a_info.Type) {}

std::shared_ptr<Graphics::SpriteTemplate> Graphics::CreateSpriteTemplate(const SpriteTemplateCreateInfo& a_info) {
	return make_shared<Graphics::SpriteTemplateImpl>(a_info);
}