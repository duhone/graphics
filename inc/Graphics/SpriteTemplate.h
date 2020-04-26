#pragma once

#include "Graphics/SpriteType.h"

#include <3rdParty/glm.h>

#include <memory>

namespace CR::Graphics {
	class SpriteTemplate {
	  public:
		SpriteTemplate()                      = default;
		virtual ~SpriteTemplate()             = default;
		SpriteTemplate(const SpriteTemplate&) = delete;
		SpriteTemplate& operator=(const SpriteTemplate&) = delete;
	};

	struct SpriteTemplateCreateInfo {
		std::string Name;
		std::shared_ptr<SpriteType> Type;
		glm::uvec2 FrameSize;
	};
	std::shared_ptr<SpriteTemplate> CreateSpriteTemplate(const SpriteTemplateCreateInfo& a_info);
}    // namespace CR::Graphics
