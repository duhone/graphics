#pragma once

#include "EngineInternal.h"
#include "vulkan/vulkan.hpp"

#include <memory>

namespace CR::Graphics {
	class VertexBuffer {
	  public:
		VertexBuffer() = default;
		VertexBuffer(uint32_t a_bytes);
		~VertexBuffer();
		VertexBuffer(VertexBuffer&) = delete;
		VertexBuffer(VertexBuffer&& a_other) noexcept;
		VertexBuffer& operator=(VertexBuffer&) = delete;
		VertexBuffer& operator                 =(VertexBuffer&& a_other) noexcept;

		// Must perform a buffer copy from staging to main anytime you have updated the buffer with new data.
		[[nodiscard]] const vk::Buffer& GetHandle() const { return m_buffer; }
		[[nodiscard]] const vk::Buffer& GetStagingHandle() const { return m_stagingBuffer; }

		[[nodiscard]] std::byte* GetData() const { return m_data; }

		template<typename T>
		[[nodiscard]] T* GetData() {
			return (T*)GetData();
		}

		[[nodiscard]] uint32_t GetSize() { return m_size; }

	  private:
		vk::Buffer m_buffer;
		vk::DeviceMemory m_bufferMemory;
		vk::Buffer m_stagingBuffer;
		vk::DeviceMemory m_stagingBufferMemory;
		std::byte* m_data{nullptr};
		uint32_t m_size;
	};
}    // namespace CR::Graphics
