﻿#pragma once

#include "core/Span.h"

#include <string>
#include <vector>

namespace CR::Graphics {
	struct TextureCreateInfo {
		std::string Name;
		Core::Span<const std::byte> TextureData;    // crtex file
	};

	class TextureSet {
	  public:
		TextureSet() = default;
		TextureSet(const Core::Span<TextureCreateInfo> a_textures);
		~TextureSet();
		TextureSet(const TextureSet&) = delete;
		TextureSet(TextureSet&& a_other) noexcept;
		TextureSet& operator=(const TextureSet&) = delete;
		TextureSet& operator                     =(TextureSet&& a_other) noexcept;

	  private:
		inline static constexpr uint16_t c_unused{0xffff};

		uint16_t m_id{c_unused};
	};
}    // namespace CR::Graphics
