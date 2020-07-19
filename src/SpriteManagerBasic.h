#pragma once

#include "CommandPool.h"
#include "DescriptorPool.h"
#include "Graphics/SpriteBasic.h"
#include "Graphics/SpriteTemplateBasic.h"
#include "Pipeline.h"
#include "UniformBufferDynamic.h"
#include "VertexBuffer.h"
#include "types/UNorm.h"

#include <3rdParty/glm.h>

#include <bitset>
#include <limits>
#include <memory>
#include <string>

namespace CR::Graphics {
	inline constexpr uint32_t c_maxSpriteTemplates = 64;
	inline constexpr uint32_t c_maxSprites         = 4096;
	static_assert(c_maxSpriteTemplates < std::numeric_limits<uint8_t>::max());    // index is a uint8_t
	static_assert(c_maxSprites < std::numeric_limits<uint16_t>::max());           // index is a uint16_t

	struct SpriteTemplates {
		std::bitset<c_maxSpriteTemplates> Used;
		uint16_t TextureIndices[c_maxSprites];
		std::string Names[c_maxSpriteTemplates];
		glm::uvec2 FrameSizes[c_maxSpriteTemplates];
		eFrameRate FrameRates[c_maxSprites];
		uint16_t MaxFrames[c_maxSprites];
	};

	struct Sprites {
		std::bitset<c_maxSprites> Used;
		std::string Names[c_maxSprites];
		std::shared_ptr<SpriteTemplateBasic> Templates[c_maxSprites];
		uint8_t TemplateIndices[c_maxSprites];
		glm::vec2 Positions[c_maxSprites];
		glm::vec4 Colors[c_maxSprites];
		float Rotations[c_maxSprites];
		uint16_t CurrentFrame[c_maxSprites];
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
		void SetSpriteRotation(uint16_t a_index, float a_rotation);

		void Frame(CommandBuffer& a_commandBuffer);

		void Draw(CommandBuffer& a_commandBuffer);

	  private:
#pragma pack(push)
#pragma pack(1)
		struct Vertex {
			glm::vec2 Offset;
			glm::u16vec2 TextureFrame;
			UNorm4<uint8_t> Color;
			glm::u16vec2 FrameSize;
			glm::vec4 Rotation;
		};
#pragma pack(pop)

		SpriteTemplates m_spriteTemplates;
		Sprites m_sprites;
		uint16_t m_currentFrame{0};
		uint32_t m_numSpritesThisFrame{0};

		Pipeline Pipeline;
		VertexBuffer<Vertex> m_vertexBuffer;
		vk::DescriptorSet DescSet;
	};

	inline void SpriteManagerBasic::SetSpritePosition(uint16_t a_index, const glm::vec2& a_position) {
		m_sprites.Positions[a_index] = a_position;
	}

	inline void SpriteManagerBasic::SetSpriteColor(uint16_t a_index, const glm::vec4& a_color) {
		m_sprites.Colors[a_index] = a_color;
	}

	inline void SpriteManagerBasic::SetSpriteRotation(uint16_t a_index, float a_rotation) {
		m_sprites.Rotations[a_index] = a_rotation;
	}
}    // namespace CR::Graphics
