#pragma once

#include "vulkan/EngineInternal.h"
#include "vulkan/vulkan.hpp"

#include <memory>

namespace CR::Graphics {
	// This currently always a host visible, coherent uniform buffer. It is always mapped, intended for data that needs
	// updating every frame
	class UniformBufferDynamic {
	  public:
		UniformBufferDynamic() = default;
		UniformBufferDynamic(uint32_t a_bytes);
		~UniformBufferDynamic();
		UniformBufferDynamic(UniformBufferDynamic&) = delete;
		UniformBufferDynamic(UniformBufferDynamic&& a_other);
		UniformBufferDynamic& operator=(UniformBufferDynamic&) = delete;
		UniformBufferDynamic& operator                         =(UniformBufferDynamic&& a_other);

		const vk::Buffer& GetHandle() const { return m_Buffer; }
		std::byte* GetData() const { return m_data; }

		template<typename T>
		T* GetData() {
			return (T*)GetData();
		}

	  private:
		void Free();

		vk::Buffer m_Buffer;
		vk::DeviceMemory m_BufferMemory;
		std::byte* m_data{nullptr};
	};
}    // namespace CR::Graphics
