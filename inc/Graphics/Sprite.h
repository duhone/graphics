#pragma once

#include "Graphics/SpriteTemplate.h"

#include <memory>

namespace CR::Graphics {
	class Sprite {
	  public:
		Sprite()              = default;
		virtual ~Sprite()     = default;
		Sprite(const Sprite&) = delete;
		Sprite& operator=(const Sprite&) = delete;
	};

	struct SpriteCreateInfo {
		std::string Name;
		std::shared_ptr<SpriteTemplate> Template;
	};
	std::unique_ptr<Sprite> CreateSprite(const SpriteCreateInfo& a_info);
}    // namespace CR::Graphics
