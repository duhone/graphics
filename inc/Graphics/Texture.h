#pragma once

#include "core/Span.h"

#include <string>

namespace CR::Graphics {
	struct TextureCreateInfo;

	class Texture {
		friend Texture CreateTexture(const TextureCreateInfo&);

	  public:
		~Texture();
		Texture(const Texture&) = delete;
		Texture(Texture&& a_other);
		Texture& operator=(const Texture&) = delete;
		Texture& operator                  =(Texture&& a_other);

	  private:
		Texture(uint16_t a_id, const std::string* a_name) : m_id(a_id), m_name(a_name) {}

		inline static constexpr uint16_t c_unused{0xffff};

		uint16_t m_id{c_unused};
		const std::string* m_name;
	};

	struct TextureCreateInfo {
		std::string Name;
		Core::Span<std::byte> TextureData;    // crtex file
	};
	Texture CreateTexture(const TextureCreateInfo& a_texture);

}    // namespace CR::Graphics
