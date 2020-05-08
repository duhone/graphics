#pragma once

#include "Graphics/SpriteTemplateBasic.h"
#include "Pipeline.h"

#include <memory>

namespace CR::Graphics {
	class SpriteTemplateBasicImpl : public SpriteTemplateBasic {
	  public:
		SpriteTemplateBasicImpl(uint8_t a_index);
		virtual ~SpriteTemplateBasicImpl();
		SpriteTemplateBasicImpl(const SpriteTemplateBasicImpl&) = delete;
		SpriteTemplateBasicImpl& operator=(const SpriteTemplateBasicImpl&) = delete;

		uint8_t GetIndex() const { return m_index; }

	  private:
		uint8_t m_index;
	};
}    // namespace CR::Graphics
