#pragma once
#include "math/Types.h"
#include <memory>

namespace CR::Graphics {
	struct Buffer {
		Buffer()        = default;
		Buffer(Buffer&) = delete;
		Buffer& operator=(Buffer&) = delete;
		virtual ~Buffer()          = default;
	};

	enum class BufferType {
		Uniform,
		Vertex,
		Index,
	};

	std::unique_ptr<Buffer> CreateBuffer(BufferType a_type, uint a_bytes);
}    // namespace CR::Graphics
