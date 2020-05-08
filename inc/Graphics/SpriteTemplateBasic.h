#pragma once

#include <3rdParty/glm.h>

#include <memory>

namespace CR::Graphics {
	class SpriteTemplateBasic {
	  public:
		SpriteTemplateBasic()                           = default;
		virtual ~SpriteTemplateBasic()                  = default;
		SpriteTemplateBasic(const SpriteTemplateBasic&) = delete;
		SpriteTemplateBasic& operator=(const SpriteTemplateBasic&) = delete;
	};

	struct SpriteTemplateBasicCreateInfo {
		std::string Name;
		glm::uvec2 FrameSize;
	};
	std::shared_ptr<SpriteTemplateBasic> CreateSpriteTemplateBasic(const SpriteTemplateBasicCreateInfo& a_info);
}    // namespace CR::Graphics
