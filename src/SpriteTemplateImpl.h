#pragma once

#include "Graphics/SpriteTemplate.h"
#include "Pipeline.h"

#include <memory>

namespace CR::Graphics {
	class SpriteTemplateImpl : public SpriteTemplate {
	  public:
		SpriteTemplateImpl()                          = default;
		virtual ~SpriteTemplateImpl()                 = default;
		SpriteTemplateImpl(const SpriteTemplateImpl&) = delete;
		SpriteTemplateImpl& operator=(const SpriteTemplateImpl&) = delete;

	  private:
	};
}    // namespace CR::Graphics