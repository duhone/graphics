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

		uint8_t GetIndex() const { return m_index; }

	  private:
		uint8_t m_index;
	};
}    // namespace CR::Graphics
