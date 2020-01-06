#pragma once

#include "Pipeline.h"

#include "glm/vec2.hpp"

#include <bitset>
#include <memory>
#include <string>

namespace CR::Graphics {
	inline constexpr uint32_t MaxSpriteTypes = 64;
	static_assert(MaxSpriteTypes <= 256);    // index is a uint8_t

	struct SpriteTypes {
		std::bitset<MaxSpriteTypes> Used;
		std::string Names[MaxSpriteTypes];
		std::unique_ptr<Pipeline> Pipelines[MaxSpriteTypes];
		glm::uvec2 TextureSizes[MaxSpriteTypes];
	};

	class SpriteManager {
	  public:
		SpriteManager();
		~SpriteManager();
		SpriteManager(const SpriteManager&) = delete;
		SpriteManager& operator=(const SpriteManager&) = delete;

		uint8_t CreateType(const std::string_view a_name, std::unique_ptr<Pipeline> a_pipeline);
		void FreeType(uint8_t a_index);

	  private:
		SpriteTypes m_spriteTypes;
	};
}    // namespace CR::Graphics
