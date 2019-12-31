#pragma once

#include "core/Span.h"

#include <cstddef>
#include <memory>
#include <vector>

namespace CR::Graphics {
	struct Pipeline {
		Pipeline()                = default;
		virtual ~Pipeline()       = default;
		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;

		virtual const std::uintptr_t GetHandle() const = 0;
	};

	struct CreatePipelineArgs {
		Core::Span<std::byte> ShaderModule;    // crsm file
	};
	std::unique_ptr<Pipeline> CreatePipeline(const CreatePipelineArgs& a_args);
}    // namespace CR::Graphics
