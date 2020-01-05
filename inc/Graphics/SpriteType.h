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

	struct SpriteTypeCreateInfo {
		std::string Name;
		Core::Span<std::byte> ShaderModule;    // crsm file
		// This is size of entire texture holding all frames, not just a single sprite frame
		// Maximum is 8Kx8K
		glm::uvec2 TextureSize;
	};
	// The number of sprite types has the largest impact on performance of your app,
	// try to keep the number of types as small as possible
	std::shared_ptr<SpriteType> CreateSpriteType(const SpriteTypeCreateInfo& a_info);
}    // namespace CR::Graphics