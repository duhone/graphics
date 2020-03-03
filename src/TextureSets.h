#pragma once

#include "CommandPool.h"
#include "vulkan/EngineInternal.h"

#include <vector>

namespace CR::Graphics::TextureSets {
	void Init();
	void Shutdown();

	uint32_t GetCurrentVersion();
	void GetImageData(std::vector<vk::ImageView>& a_images, std::vector<uint16_t>& a_imageIndices);

	void CheckLoadingTasks(CommandBuffer& a_cmdBuffer);
}    // namespace CR::Graphics::TextureSets
