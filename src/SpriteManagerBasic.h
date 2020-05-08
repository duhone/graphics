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
	inline constexpr uint32_t SpritesPerTemplate = 128;
	inline constexpr uint32_t MaxSpriteTemplates = 64;
	inline constexpr uint32_t MaxSprites         = MaxSpriteTemplates * SpritesPerTemplate;
	static_assert(MaxSpriteTemplates - 1 <= std::numeric_limits<uint8_t>::max());    // index is a uint8_t
	static_assert(MaxSprites - 1 <= std::numeric_limits<uint16_t>::max());           // index is a uint16_t

	struct SpriteTemplates {
		std::bitset<MaxSpriteTemplates> Used;
		std::string Names[MaxSpriteTemplates];
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
		std::shared_ptr<SpriteTemplateBasic> Templates[MaxSprites];
		uint8_t TemplateIndices[MaxSprites];
		glm::vec2 Positions[MaxSprites];
		glm::vec4 Colors[MaxSprites];
		uint16_t TextureIndices[MaxSprites];
		SpriteBasic::eFrameRate FrameRates[MaxSprites];
		uint16_t MaxFrames[MaxSprites];
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

		uint8_t CreateTemplate(std::string_view a_name, const glm::uvec2& a_frameSize);
		void FreeTemplate(uint8_t a_index);

		uint16_t CreateSprite(std::string_view a_name, std::shared_ptr<SpriteTemplateBasic> a_template,
		                      const char* a_textureName);
		void FreeSprite(uint16_t a_index);
		void SetSprite(uint16_t a_index, const glm::vec2& a_position, const glm::vec4& a_color,
		               SpriteBasic::eFrameRate a_framerate);

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

	inline void SpriteManagerBasic::SetSprite(uint16_t a_index, const glm::vec2& a_position, const glm::vec4& a_color,
	                                          SpriteBasic::eFrameRate a_framerate) {
		m_sprites.Positions[a_index]  = a_position;
		m_sprites.Colors[a_index]     = a_color;
		m_sprites.FrameRates[a_index] = a_framerate;
	}
}    // namespace CR::Graphics
