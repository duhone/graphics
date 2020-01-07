#pragma once

#include "Graphics/SpriteTemplate.h"
#include "Pipeline.h"

#include <memory>

namespace CR::Graphics {
	class SpriteTemplateImpl : public SpriteTemplate {
	  public:
		SpriteTemplateImpl(uint8_t a_index);
		virtual ~SpriteTemplateImpl();
		SpriteTemplateImpl(const SpriteTemplateImpl&) = delete;
		SpriteTemplateImpl& operator=(const SpriteTemplateImpl&) = delete;

		uint8_t GetIndex() const { return m_index; }

	  private:
		uint8_t m_index;
	};
}    // namespace CR::Graphics
