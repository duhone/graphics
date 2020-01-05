#pragma once

#include <memory>

namespace CR::Graphics {
	class SpriteTemplate {
	  public:
		SpriteTemplate()                      = default;
		virtual ~SpriteTemplate()             = default;
		SpriteTemplate(const SpriteTemplate&) = delete;
		SpriteTemplate& operator=(const SpriteTemplate&) = delete;
	};

	std::unique_ptr<SpriteTemplate> CreateSpriteTemplate();
}    // namespace CR::Graphics