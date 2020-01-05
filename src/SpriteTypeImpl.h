#pragma once

#include "Graphics/SpriteType.h"
#include "Pipeline.h"

#include <memory>

namespace CR::Graphics {
	class SpriteTypeImpl : public SpriteType {
	  public:
		SpriteTypeImpl(const SpriteTypeCreateInfo& a_info);
		virtual ~SpriteTypeImpl()             = default;
		SpriteTypeImpl(const SpriteTypeImpl&) = delete;
		SpriteTypeImpl& operator=(const SpriteTypeImpl&) = delete;

	  private:
		std::string m_name;
		std::unique_ptr<Pipeline> m_pipeline;
		glm::uvec2 m_textureSize;
	};
}    // namespace CR::Graphics