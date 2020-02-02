#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOCOMM
#define NOMINMAX
#include "vulkan/vulkan.hpp"
#include <windows.h>

#include "glm/vec2.hpp"

#include <functional>

namespace CR::Graphics {
	class SpriteManager;

	void GetDevice(std::function<void(vk::Device&)> a_func);
	uint32_t GetDeviceMemoryIndex();
	uint32_t GetHostMemoryIndex();
	const vk::RenderPass& GetRenderPass();
	const vk::Framebuffer& GetFrameBuffer();

	uint32_t GetGraphicsQueueIndex();
	uint32_t GetTransferQueueIndex();

	vk::Queue& GetTransferQueue();

	const glm::ivec2& GetWindowSize();
	SpriteManager& GetSpriteManager();

	void ExecuteNextFrame(std::function<void()> a_func);
}    // namespace CR::Graphics
