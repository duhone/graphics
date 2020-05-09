#pragma once

#include <3rdParty/glm.h>

#include <memory>
#include <string>

namespace CR::Graphics {
	class SpriteTemplateBasic {
	  public:
		SpriteTemplateBasic()                           = default;
		virtual ~SpriteTemplateBasic()                  = default;
		SpriteTemplateBasic(const SpriteTemplateBasic&) = delete;
		SpriteTemplateBasic(SpriteTemplateBasic&&)      = delete;
		SpriteTemplateBasic& operator=(const SpriteTemplateBasic&) = delete;
		SpriteTemplateBasic& operator=(SpriteTemplateBasic&&) = delete;
	};

	enum class eFrameRate { None, FPS10, FPS12, FPS15, FPS20, FPS30, FPS60 };

	struct SpriteTemplateBasicCreateInfo {
		std::string Name;
		std::string TextureName;
		glm::uvec2 FrameSize;
		eFrameRate FrameRate;
	};
	std::shared_ptr<SpriteTemplateBasic> CreateSpriteTemplateBasic(const SpriteTemplateBasicCreateInfo& a_info);
}    // namespace CR::Graphics
