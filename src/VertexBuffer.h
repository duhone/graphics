#pragma once

#include "EngineInternal.h"
#include "Formats.h"

#include <memory>
#include <vector>

namespace CR::Graphics {
	// Source vertex struct must be tightly packed.
	class VertexBufferLayout {
	  public:
		VertexBufferLayout()                             = default;
		~VertexBufferLayout()                            = default;
		VertexBufferLayout(VertexBufferLayout&)          = delete;
		VertexBufferLayout(VertexBufferLayout&& a_other) = delete;
		VertexBufferLayout& operator=(VertexBufferLayout&) = delete;
		VertexBufferLayout& operator=(VertexBufferLayout&& a_other) = delete;

		// Someday we will have reflection. For now create a dummy struct of your vertex layout(must be the proper types
		// that match your compressed layout, UNorm2, SNorm4, ect), and then pass each member variable, in order to this
		// function(error prone, but aimed at switching to reflection some day). In your shader, the input locations
		// must be in same order as the struct.
		template<typename T>
		void AddVariable(const T& a_var) noexcept;

	  private:
		struct Entry {
			vk::Format format{vk::Format::eUndefined};
			uint16_t Offset{0};
			uint8_t Location{0};
		};
		std::vector<Entry> m_layout;
		uint8_t m_nextLocation{0};
		uint16_t m_nextOffset{0};
	};

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
		[[nodiscard]] const vk::Buffer& GetHandle() const noexcept { return m_buffer; }
		[[nodiscard]] const vk::Buffer& GetStagingHandle() const noexcept { return m_stagingBuffer; }

		[[nodiscard]] std::byte* GetData() const noexcept { return m_data; }

		template<typename T>
		[[nodiscard]] T* GetData() noexcept {
			return (T*)GetData();
		}

		[[nodiscard]] uint32_t GetSize() const noexcept { return m_size; }

	  private:
		vk::Buffer m_buffer;
		vk::DeviceMemory m_bufferMemory;
		vk::Buffer m_stagingBuffer;
		vk::DeviceMemory m_stagingBufferMemory;
		std::byte* m_data{nullptr};
		uint32_t m_size;
	};

	template<typename T>
	void VertexBufferLayout::AddVariable(const T& a_var) noexcept {
		Entry entry;
		entry.Location = m_nextLocation++;
		entry.Offset   = m_nextOffset;
		m_nextOffset += sizeof(a_var);
		entry.format = GetVKFormat(a_var);
		m_layout.push_back(entry);
	}
}    // namespace CR::Graphics
