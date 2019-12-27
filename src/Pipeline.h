#pragma once
#include <memory>

namespace CR::Graphics {
	struct Pipeline {
		Pipeline()                = default;
		~Pipeline()               = default;
		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;
	};

	std::unique_ptr<Pipeline> CreatePipeline();
}    // namespace CR::Graphics
