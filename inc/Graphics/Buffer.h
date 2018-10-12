#pragma once
#include "math/Types.h"
#include <memory>

namespace CR::Graphics {
	struct Buffer {
		Buffer()        = default;
		Buffer(Buffer&) = delete;
		Buffer& operator=(Buffer&) = delete;
		virtual ~Buffer()          = default;

		virtual void* Map()  = 0;
		virtual void UnMap() = 0;

		template<typename T>
		T* Map() {
			return (T*)Map();
		}
	};

	enum class BufferType {
		Uniform,
		Vertex,
		Index,
	};

	std::unique_ptr<Buffer> CreateBuffer(BufferType a_type, uint a_bytes);
}    // namespace CR::Graphics
