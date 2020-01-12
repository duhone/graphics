#pragma once

#include "vulkan/EngineInternal.h"
#include "vulkan/vulkan.hpp"

#include <memory>

namespace CR::Graphics {
	enum class BufferType {
		Uniform,
		Vertex,
		Index,
	};

	class Buffer {
	  public:
		Buffer() = default;
		Buffer(BufferType a_type, uint32_t a_bytes);
		~Buffer();
		Buffer(Buffer&) = delete;
		Buffer(Buffer&& a_other);
		Buffer& operator=(Buffer&) = delete;
		Buffer& operator           =(Buffer&& a_other);

		const vk::Buffer& GetHandle() const { return m_Buffer; }
		std::byte* Map();
		void UnMap();

		template<typename T>
		T* Map() {
			return (T*)Map();
		}

	  private:
		void Free();

		BufferType m_type;
		vk::Buffer m_Buffer;
		vk::Buffer m_StagingBuffer;
		vk::DeviceMemory m_BufferMemory;
		vk::DeviceMemory m_StagingBufferMemory;
	};
}    // namespace CR::Graphics
