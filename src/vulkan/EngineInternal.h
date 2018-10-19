#pragma once
#include "vulkan/vulkan.hpp"

namespace CR::Graphics {
	vk::Device& GetDevice();
	uint32_t GetDeviceMemoryIndex();
	uint32_t GetHostMemoryIndex();

	uint32_t GetGraphicsQueueIndex();
	uint32_t GetTransferQueueIndex();
	vk::Queue& GetGraphicsQueue();
	vk::Queue& GetTransferQueue();
}    // namespace CR::Graphics
