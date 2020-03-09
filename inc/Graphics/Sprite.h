#pragma once

#include "Graphics/SpriteTemplate.h"

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

#include <memory>
#include <string>

namespace CR::Graphics {
	class Sprite {
	  public:
		Sprite()              = default;
		virtual ~Sprite()     = default;
		Sprite(const Sprite&) = delete;
		Sprite& operator=(const Sprite&) = delete;

		struct Props {
			glm::vec2 Position;
			glm::vec4 Color;
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
