#pragma once

#include "vulkan/EngineInternal.h"

#include "function2/function2.hpp"

#include <future>
#include <memory>

namespace CR::Graphics {
	struct CommandBuffer;
}

namespace CR::Graphics::AssetLoadingThread {
	using task_t = fu2::unique_function<void(CommandBuffer&)>;

	void Init();
	void Shutdown();

	std::shared_ptr<std::atomic_bool> LoadAsset(task_t&& a_task);

}    // namespace CR::Graphics::AssetLoadingThread
