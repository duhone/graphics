#pragma once

#include "CommandPool.h"
#include "EngineInternal.h"

#include <string_view>
#include <vector>

namespace CR::Graphics::TextureSets {
	void Init();
	void Shutdown();

	uint32_t GetCurrentVersion();
	void GetImageData(std::vector<vk::ImageView>& a_images, std::vector<uint16_t>& a_imageIndices);
	uint16_t GetTextureIndex(const char* a_textureName);
	uint16_t GetMaxFrames(uint16_t a_textureIndex);

	void CheckLoadingTasks(CommandBuffer& a_cmdBuffer);
}    // namespace CR::Graphics::TextureSets
