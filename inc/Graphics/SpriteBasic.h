#pragma once

#include "Graphics/SpriteTemplateBasic.h"

#include <3rdParty/glm.h>

#include <memory>
#include <string>

namespace CR::Graphics {
	class SpriteBasic {
	  public:
		SpriteBasic()                   = default;
		virtual ~SpriteBasic()          = default;
		SpriteBasic(const SpriteBasic&) = delete;
		SpriteBasic& operator=(const SpriteBasic&) = delete;

		enum class eFrameRate { None, FPS10, FPS12, FPS15, FPS20, FPS30, FPS60 };
		struct Props {
			glm::vec2 Position;
			glm::vec4 Color;
			eFrameRate FrameRate{eFrameRate::None};
		};
		virtual void SetProps(const Props& a_props) = 0;
	};

	struct SpriteBasicCreateInfo {
		std::string Name;
		std::shared_ptr<SpriteTemplateBasic> Template;
		std::string TextureName;
	};
	std::unique_ptr<SpriteBasic> CreateSpriteBasic(const SpriteBasicCreateInfo& a_info);
}    // namespace CR::Graphics
