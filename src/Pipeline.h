#pragma once

#include "core/Span.h"

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

	struct CreatePipelineArgs {
		Core::Span<std::byte> ShaderModule;    // crsm file
	};
	std::unique_ptr<Pipeline> CreatePipeline(const CreatePipelineArgs& a_args);
}    // namespace CR::Graphics
