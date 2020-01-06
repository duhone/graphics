#pragma once

#include "Graphics/SpriteType.h"
#include "Pipeline.h"

#include <memory>

namespace CR::Graphics {
	class SpriteTypeImpl : public SpriteType {
	  public:
		SpriteTypeImpl(uint8_t a_index);
		virtual ~SpriteTypeImpl();
		SpriteTypeImpl(const SpriteTypeImpl&) = delete;
		SpriteTypeImpl& operator=(const SpriteTypeImpl&) = delete;

	  private:
		uint8_t m_index;
	};
}    // namespace CR::Graphics
