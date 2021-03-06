﻿#pragma once

#include "CommandPool.h"
#include "EngineInternal.h"
#include "Event.h"
#include "Formats.h"

#include <memory>
#include <vector>

namespace CR::Graphics {
	namespace detail {
		class VertexBufferBase;
	}

	// Source vertex struct must be tightly packed.
	class VertexBufferLayout {
		friend detail::VertexBufferBase;

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

		[[nodiscard]] uint32_t GetStride() const noexcept { return m_nextOffset; }

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

	namespace detail {
		class VertexBufferBase {
		  public:
			VertexBufferBase() = default;
			VertexBufferBase(const VertexBufferLayout& a_layout, uint32_t a_vertCount, void** a_data);
			~VertexBufferBase();
			VertexBufferBase(VertexBufferBase&) = delete;
			VertexBufferBase(VertexBufferBase&& a_other) noexcept;
			VertexBufferBase& operator=(VertexBufferBase&) = delete;
			VertexBufferBase& operator                     =(VertexBufferBase&& a_other) noexcept;

			[[nodiscard]] const vk::VertexInputBindingDescription& GetBindingDescription() const noexcept {
				return m_bindingDescription;
			}
			[[nodiscard]] const std::vector<vk::VertexInputAttributeDescription>& GetAttrDescriptions() const noexcept {
				return m_attrDescriptions;
			}

			void Release(CommandBuffer& a_cmdBuffer, uint32_t a_sizeBytes);
			void Acquire(CommandBuffer& a_cmdBuffer);

			[[nodiscard]] const vk::Buffer& GetHandle() const noexcept { return m_buffer; }

		  private:
			void Free();

			vk::Buffer m_buffer;
			vk::DeviceMemory m_bufferMemory;
			vk::Buffer m_stagingBuffer;
			vk::DeviceMemory m_stagingBufferMemory;

			Event m_copyEvent;

			vk::VertexInputBindingDescription m_bindingDescription;
			std::vector<vk::VertexInputAttributeDescription> m_attrDescriptions;
		};
	}    // namespace detail

	template<typename T>
	class VertexBuffer {
	  public:
		VertexBuffer() = default;
		VertexBuffer(const VertexBufferLayout& a_layout, uint32_t a_vertCount) :
		    m_base(a_layout, a_vertCount, (void**)&m_data) {
			m_size   = a_vertCount;
			m_stride = a_layout.GetStride();
		}
		~VertexBuffer()                = default;
		VertexBuffer(VertexBuffer<T>&) = delete;
		VertexBuffer(VertexBuffer<T>&& a_other) noexcept;
		VertexBuffer& operator=(VertexBuffer<T>&) = delete;
		VertexBuffer& operator                    =(VertexBuffer<T>&& a_other) noexcept;

		[[nodiscard]] const vk::Buffer& GetHandle() const noexcept { return m_base.GetHandle(); }

		[[nodiscard]] T* begin() noexcept { return m_data; }
		[[nodiscard]] const T* begin() const noexcept { return m_data; }
		[[nodiscard]] const T* cbegin() const noexcept { return m_data; }
		[[nodiscard]] T* end() noexcept { return m_data + m_size; }
		[[nodiscard]] const T* end() const noexcept { return m_data + m_size; }
		[[nodiscard]] const T* cend() const noexcept { return m_data + m_size; }

		[[nodiscard]] bool empty() const noexcept { return m_size == 0; }

		[[nodiscard]] const vk::VertexInputBindingDescription& GetBindingDescription() const noexcept {
			return m_base.GetBindingDescription();
		}

		[[nodiscard]] const std::vector<vk::VertexInputAttributeDescription>& GetAttrDescriptions() const noexcept {
			return m_base.GetAttrDescriptions();
		}

		// Must release the buffer after all writes for the current frame are done, then later you must acquire the
		// buffer before any commands are issued that use it. Seperate the 2 calls by as much time as possible to avoid
		// pipeline stalls.
		void Release(CommandBuffer& a_cmdBuffer) { m_base.Release(a_cmdBuffer, m_size * m_stride); }
		void Acquire(CommandBuffer& a_cmdBuffer) { m_base.Acquire(a_cmdBuffer); }

	  private:
		T* m_data{nullptr};
		uint32_t m_size{0};
		uint32_t m_stride{0};

		// must be last, so it initializes last
		detail::VertexBufferBase m_base;
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

	template<typename T>
	VertexBuffer<T>::VertexBuffer(VertexBuffer<T>&& a_other) noexcept {
		*this = move(a_other);
	}

	template<typename T>
	VertexBuffer<T>& VertexBuffer<T>::operator=(VertexBuffer<T>&& a_other) noexcept {
		m_base   = std::move(a_other.m_base);
		m_data   = a_other.m_data;
		m_size   = a_other.m_size;
		m_stride = a_other.m_stride;

		a_other.m_data = nullptr;
		a_other.m_size = 0;

		return *this;
	}
}    // namespace CR::Graphics
