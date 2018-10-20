#pragma once
#include "vulkan/vulkan.hpp"

namespace CR::Graphics {
	vk::Device& GetDevice();
	uint32_t GetDeviceMemoryIndex();
	uint32_t GetHostMemoryIndex();

	uint32_t GetGraphicsQueueIndex();
	uint32_t GetTransferQueueIndex();
	void SubmitGraphicsCommands(const std::vector<vk::CommandBuffer>& cmds);
	void SubmitTransferCommands(const std::vector<vk::CommandBuffer>& cmds);
}    // namespace CR::Graphics
