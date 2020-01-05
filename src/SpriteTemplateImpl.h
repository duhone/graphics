#pragma once

#include "Graphics/SpriteTemplate.h"
#include "Pipeline.h"

#include <memory>

namespace CR::Graphics {
	class SpriteTemplateImpl : public SpriteTemplate {
	  public:
		SpriteTemplateImpl(const SpriteTemplateCreateInfo& a_info);
		virtual ~SpriteTemplateImpl()                 = default;
		SpriteTemplateImpl(const SpriteTemplateImpl&) = delete;
		SpriteTemplateImpl& operator=(const SpriteTemplateImpl&) = delete;

	  private:
		std::string m_name;
		std::shared_ptr<SpriteType> m_type;
	};
}    // namespace CR::Graphics