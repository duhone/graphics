#pragma once

#include <memory>

namespace CR::Graphics {
	class Sprite {
	  public:
		Sprite()              = default;
		virtual ~Sprite()     = default;
		Sprite(const Sprite&) = delete;
		Sprite& operator=(const Sprite&) = delete;
	};

	std::unique_ptr<Sprite> CreateSprite();
}    // namespace CR::Graphics