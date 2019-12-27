#pragma once
#include <cstddef>
#include <memory>
#include <vector>

namespace CR::Graphics {
	struct Pipeline {
		Pipeline()                = default;
		~Pipeline()               = default;
		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;
	};

	std::unique_ptr<Pipeline> CreatePipeline();
}    // namespace CR::Graphics
