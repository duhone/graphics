#pragma once

#include "core/Span.h"

#include <string>
#include <vector>

namespace CR::Graphics {
	struct TextureCreateInfo;

	class TextureSet {
		friend TextureSet CreateTextureSet(const Core::Span<TextureCreateInfo>);

	  public:
		~TextureSet();
		TextureSet(const TextureSet&) = delete;
		TextureSet(TextureSet&& a_other);
		TextureSet& operator=(const TextureSet&) = delete;
		TextureSet& operator                     =(TextureSet&& a_other);

	  private:
		TextureSet(uint16_t a_id) : m_id(a_id) {}

		inline static constexpr uint16_t c_unused{0xffff};

		uint16_t m_id{c_unused};
	};

	struct TextureCreateInfo {
		std::string Name;
		Core::Span<const std::byte> TextureData;    // crtex file
	};
	TextureSet CreateTextureSet(const Core::Span<TextureCreateInfo> a_textures);

}    // namespace CR::Graphics
