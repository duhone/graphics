#pragma once

#include "CommandPool.h"
#include "Graphics/SpriteTemplate.h"
#include "Graphics/SpriteType.h"
#include "Pipeline.h"

#include "glm/vec2.hpp"

#include <bitset>
#include <limits>
#include <memory>
#include <string>

namespace CR::Graphics {
	inline constexpr uint32_t MaxSpriteTypes     = 16;
	inline constexpr uint32_t MaxSpriteTemplates = 64;
	inline constexpr uint32_t MaxSprites         = 512;
	static_assert(MaxSpriteTypes <= std::numeric_limits<uint8_t>::max());        // index is a uint8_t
	static_assert(MaxSpriteTemplates <= std::numeric_limits<uint8_t>::max());    // index is a uint8_t
	static_assert(MaxSprites <= std::numeric_limits<uint16_t>::max());           // index is a uint16_t

	struct SpriteTypes {
		std::bitset<MaxSpriteTypes> Used;
		std::string Names[MaxSpriteTypes];
		std::unique_ptr<Pipeline> Pipelines[MaxSpriteTypes];
		glm::uvec2 TextureSizes[MaxSpriteTypes];
	};

	struct SpriteTemplates {
		std::bitset<MaxSpriteTemplates> Used;
		std::string Names[MaxSpriteTemplates];
		std::shared_ptr<SpriteType> Types[MaxSpriteTemplates];
		uint8_t TypeIndices[MaxSpriteTemplates];
	};

	struct Sprites {
		std::bitset<MaxSprites> Used;
		std::string Names[MaxSprites];
		std::shared_ptr<SpriteTemplate> Templates[MaxSprites];
		uint16_t TemplateIndices[MaxSprites];
	};

	class SpriteManager {
	  public:
		SpriteManager();
		~SpriteManager();
		SpriteManager(const SpriteManager&) = delete;
		SpriteManager& operator=(const SpriteManager&) = delete;

		uint8_t CreateType(const std::string_view a_name, std::unique_ptr<Pipeline> a_pipeline);
		void FreeType(uint8_t a_index);

		uint8_t CreateTemplate(const std::string_view a_name, std::shared_ptr<SpriteType> a_type);
		void FreeTemplate(uint8_t a_index);

		uint16_t CreateSprite(const std::string_view a_name, std::shared_ptr<SpriteTemplate> a_template);
		void FreeSprite(uint16_t a_index);

		void Draw(CommandBuffer& a_commandBuffer);

	  private:
		SpriteTypes m_spriteTypes;
		SpriteTemplates m_spriteTemplates;
		Sprites m_sprites;
	};
}    // namespace CR::Graphics
