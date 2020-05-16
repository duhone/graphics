#pragma once

#include "CommandPool.h"
#include "DescriptorPool.h"
#include "Graphics/SpriteBasic.h"
#include "Graphics/SpriteTemplateBasic.h"
#include "Pipeline.h"
#include "UniformBufferDynamic.h"

#include <3rdParty/glm.h>

#include <bitset>
#include <limits>
#include <memory>
#include <string>

namespace CR::Graphics {
	// inline constexpr uint32_t SpritesPerTemplate = 128;
	inline constexpr uint32_t MaxSpriteTemplates = 64;
	inline constexpr uint32_t MaxSprites         = 1024;
	inline constexpr uint32_t MaxSpritesPerBatch = 256;
	static_assert(MaxSpriteTemplates < std::numeric_limits<uint8_t>::max());    // index is a uint8_t
	static_assert(MaxSprites < std::numeric_limits<uint16_t>::max());           // index is a uint16_t

	struct SpriteTemplates {
		std::bitset<MaxSpriteTemplates> Used;
		uint16_t TextureIndices[MaxSprites];
		std::string Names[MaxSpriteTemplates];
		glm::uvec2 FrameSizes[MaxSpriteTemplates];
		eFrameRate FrameRates[MaxSprites];
		uint16_t MaxFrames[MaxSprites];
	};

	struct SpriteUniformData {
		glm::vec4 Position;    // z and w are unused.
		glm::vec4 Color;
		glm::vec4 FrameSize;
	};
	static_assert((sizeof(SpriteUniformData) * MaxSprites) / MaxSpriteTemplates < 64 * 1024, "64KB max uniform range");

	struct Sprites {
		std::bitset<MaxSprites> Used;
		std::string Names[MaxSprites];
		std::shared_ptr<SpriteTemplateBasic> Templates[MaxSprites];
		uint8_t TemplateIndices[MaxSprites];
		glm::vec2 Positions[MaxSprites];
		glm::vec4 Colors[MaxSprites];
		uint16_t CurrentFrame[MaxSprites];
	};

	class SpriteManagerBasic {
	  public:
		SpriteManagerBasic();
		~SpriteManagerBasic();
		SpriteManagerBasic(const SpriteManagerBasic&) = delete;
		SpriteManagerBasic& operator=(const SpriteManagerBasic&) = delete;
		SpriteManagerBasic(SpriteManagerBasic&&)                 = delete;
		SpriteManagerBasic& operator=(SpriteManagerBasic&&) = delete;

		uint8_t CreateTemplate(std::string_view a_name, const glm::uvec2& a_frameSize, eFrameRate frameRate,
		                       const char* a_textureName);
		void FreeTemplate(uint8_t a_index);

		uint16_t CreateSprite(std::string_view a_name, std::shared_ptr<SpriteTemplateBasic> a_template);
		void FreeSprite(uint16_t a_index);
		void SetSpritePosition(uint16_t a_index, const glm::vec2& a_position);
		void SetSpriteColor(uint16_t a_index, const glm::vec4& a_color);

		void Frame();

		void Draw(CommandBuffer& a_commandBuffer);

	  private:
		SpriteTemplates m_spriteTemplates;
		Sprites m_sprites;
		uint16_t m_currentFrame{0};

		Pipeline Pipeline;
		UniformBufferDynamic UniformBuffer;
		vk::DescriptorSet DescSet;
	};

	inline void SpriteManagerBasic::SetSpritePosition(uint16_t a_index, const glm::vec2& a_position) {
		m_sprites.Positions[a_index] = a_position;
	}

	inline void SpriteManagerBasic::SetSpriteColor(uint16_t a_index, const glm::vec4& a_color) {
		m_sprites.Colors[a_index] = a_color;
	}
}    // namespace CR::Graphics
