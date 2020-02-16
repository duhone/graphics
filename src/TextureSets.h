#pragma once

#include "CommandPool.h"

namespace CR::Graphics::TextureSets {
	void Init();
	void Shutdown();

	void CheckLoadingTasks(CommandBuffer& a_cmdBuffer);
}    // namespace CR::Graphics::TextureSets
