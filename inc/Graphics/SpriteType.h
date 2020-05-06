#pragma once

#include "core/Span.h"

#include "glm/vec2.hpp"

#include <memory>
#include <string>

namespace CR::Graphics {
	class SpriteType {
	  public:
		SpriteType()                  = default;
		virtual ~SpriteType()         = default;
		SpriteType(const SpriteType&) = delete;
		SpriteType& operator=(const SpriteType&) = delete;
	};

	// The number of sprite types has the largest impact on performance of your app,
	// try to keep the number of types as small as possible
	std::shared_ptr<SpriteType> CreateSpriteType();
}    // namespace CR::Graphics
