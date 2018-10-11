#pragma once
#include "vulkan/vulkan.hpp"

namespace CR::Graphics {
	vk::Device& GetDevice();
	uint32_t GetDeviceMemoryIndex();
	uint32_t GetHostMemoryIndex();
}    // namespace CR::Graphics
