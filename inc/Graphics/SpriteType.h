#pragma once

#include <memory>

namespace CR::Graphics {
	class SpriteType {
	  public:
		SpriteType()                  = default;
		virtual ~SpriteType()         = default;
		SpriteType(const SpriteType&) = delete;
		SpriteType& operator=(const SpriteType&) = delete;
	};

	std::unique_ptr<SpriteType> CreateSpriteType();
}    // namespace CR::Graphics