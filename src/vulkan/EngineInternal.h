#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOCOMM
#define NOMINMAX
#include "vulkan/vulkan.hpp"
#include <windows.h>

#include "glm/vec2.hpp"

namespace CR::Graphics {
	vk::Device& GetDevice();
	uint32_t GetDeviceMemoryIndex();
	uint32_t GetHostMemoryIndex();
	const vk::RenderPass& GetRenderPass();
	const vk::Framebuffer& GetFrameBuffer();

	uint32_t GetGraphicsQueueIndex();
	uint32_t GetTransferQueueIndex();
	void SubmitGraphicsCommands(const std::vector<vk::CommandBuffer>& cmds);
	void SubmitTransferCommands(const std::vector<vk::CommandBuffer>& cmds);

	const glm::ivec2& GetWindowSize();

}    // namespace CR::Graphics
