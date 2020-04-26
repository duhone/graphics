#pragma once

#include "Graphics/SpriteTemplate.h"

#include <3rdParty/glm.h>

#include <memory>
#include <string>

namespace CR::Graphics {
	class Sprite {
	  public:
		Sprite()              = default;
		virtual ~Sprite()     = default;
		Sprite(const Sprite&) = delete;
		Sprite& operator=(const Sprite&) = delete;

		enum class eFrameRate { None, FPS10, FPS12, FPS15, FPS20, FPS30, FPS60 };
		struct Props {
			glm::vec2 Position;
			glm::vec4 Color;
			eFrameRate FrameRate{eFrameRate::None};
		};
		virtual void SetProps(const Props& a_props) = 0;
	};

	struct SpriteCreateInfo {
		std::string Name;
		std::shared_ptr<SpriteTemplate> Template;
		std::string TextureName;
	};
	std::unique_ptr<Sprite> CreateSprite(const SpriteCreateInfo& a_info);
}    // namespace CR::Graphics
