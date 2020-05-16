#pragma once

#include "Graphics/SpriteTemplateBasic.h"

#include <3rdParty/glm.h>

#include <memory>
#include <string>

namespace CR::Graphics {
	struct SpriteBasicCreateInfo {
		std::string Name;
		std::shared_ptr<SpriteTemplateBasic> Template;
	};

	// defaults to solid white, and located at 0.0,0.0
	class SpriteBasic {
	  public:
		SpriteBasic() = default;
		SpriteBasic(const SpriteBasicCreateInfo& a_info);
		~SpriteBasic();
		SpriteBasic(const SpriteBasic&) = delete;
		SpriteBasic(SpriteBasic&& a_other) noexcept;
		SpriteBasic& operator=(const SpriteBasic&) = delete;
		SpriteBasic& operator                      =(SpriteBasic&& a_other) noexcept;

		void SetPosition(const glm::vec2& a_position);
		void SetColor(const glm::vec4& a_color);

		[[nodiscard]] uint16_t GetIndex() const { return m_index; }

	  private:
		inline static constexpr uint16_t c_unused{0xffff};

		uint16_t m_index = c_unused;
	};
}    // namespace CR::Graphics
