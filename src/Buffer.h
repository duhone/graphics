#pragma once
#include <memory>

namespace CR::Graphics {
	struct Buffer {
		Buffer()        = default;
		Buffer(Buffer&) = delete;
		Buffer& operator=(Buffer&) = delete;
		virtual ~Buffer()          = default;

		virtual void* GetHandle() = 0;
		virtual void* Map()       = 0;
		virtual void UnMap()      = 0;

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

	std::unique_ptr<Buffer> CreateBuffer(BufferType a_type, uint32_t a_bytes);
}    // namespace CR::Graphics
