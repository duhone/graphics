#pragma once

#include "CommandPool.h"
#include "Graphics/SpriteTemplate.h"
#include "Graphics/SpriteType.h"
#include "Pipeline.h"
#include "UniformBufferDynamic.h"

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

#include <bitset>
#include <limits>
#include <memory>
#include <string>

namespace CR::Graphics {
	inline constexpr uint32_t MaxSpriteTypes         = 4;
	inline constexpr uint32_t SpriteTemplatesPerType = 64;
	inline constexpr uint32_t SpritesPerTemplate     = 256;
	inline constexpr uint32_t MaxSpriteTemplates     = MaxSpriteTypes * SpriteTemplatesPerType;
	inline constexpr uint32_t MaxSprites             = MaxSpriteTemplates * SpritesPerTemplate;
	static_assert(MaxSpriteTypes - 1 <= std::numeric_limits<uint8_t>::max());        // index is a uint8_t
	static_assert(MaxSpriteTemplates - 1 <= std::numeric_limits<uint8_t>::max());    // index is a uint8_t
	static_assert(MaxSprites - 1 <= std::numeric_limits<uint16_t>::max());           // index is a uint16_t

	struct SpriteTypes {
		std::bitset<MaxSpriteTypes> Used;
		std::string Names[MaxSpriteTypes];
		Pipeline Pipelines[MaxSpriteTypes];
		glm::uvec2 TextureSizes[MaxSpriteTypes];
	};

	struct SpriteTemplates {
		std::bitset<MaxSpriteTemplates> Used;
		std::string Names[MaxSpriteTemplates];
		std::shared_ptr<SpriteType> Types[MaxSpriteTemplates];
		uint8_t TypeIndices[MaxSpriteTemplates];
		glm::uvec2 FrameSizes[MaxSpriteTemplates];
	};

	struct SpriteUniformData {
		glm::vec4 Position;    // z and w are unused.
		glm::vec4 Color;
	};
	static_assert((sizeof(SpriteUniformData) * MaxSprites) / MaxSpriteTemplates < 64 * 1024, "64KB max uniform range");

	struct Sprites {
		std::bitset<MaxSprites> Used;
		std::string Names[MaxSprites];
		std::shared_ptr<SpriteTemplate> Templates[MaxSprites];
		uint8_t TemplateIndices[MaxSprites];
		glm::vec2 Positions[MaxSprites];
		glm::vec4 Colors[MaxSprites];
		UniformBufferDynamic UniformBuffer;
	};

	class SpriteManager {
	  public:
		SpriteManager();
		~SpriteManager();
		SpriteManager(const SpriteManager&) = delete;
		SpriteManager& operator=(const SpriteManager&) = delete;

		uint8_t CreateType(const std::string_view a_name, Pipeline&& a_pipeline);
		void FreeType(uint8_t a_index);

		uint8_t CreateTemplate(const std::string_view a_name, std::shared_ptr<SpriteType> a_type,
		                       const glm::uvec2& a_frameSize);
		void FreeTemplate(uint8_t a_index);

		uint16_t CreateSprite(const std::string_view a_name, std::shared_ptr<SpriteTemplate> a_template);
		void FreeSprite(uint16_t a_index);
		void SetSprite(uint16_t a_index, const glm::vec2& a_position, const glm::vec4& a_color);

		void Draw(CommandBuffer& a_commandBuffer);

	  private:
		SpriteTypes m_spriteTypes;
		SpriteTemplates m_spriteTemplates;
		Sprites m_sprites;
	};

	inline void SpriteManager::SetSprite(uint16_t a_index, const glm::vec2& a_position, const glm::vec4& a_color) {
		m_sprites.Positions[a_index] = a_position;
		m_sprites.Colors[a_index]    = a_color;
	}
}    // namespace CR::Graphics
