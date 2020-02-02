#pragma once

#include "vulkan/EngineInternal.h"

#include "function2/function2.hpp"

#include <future>
#include <memory>

namespace CR::Graphics {
	struct CommandBuffer;
}

namespace CR::Graphics::AssetLoadingThread {
	// Use the return value to free any resources. The returned function will be executed after the command buffer has
	// finished executing on the gpu. Note that LoadAsset result will be marked ready before the returned function
	// executes.
	using task_t = fu2::unique_function<fu2::unique_function<void()>(CommandBuffer&)>;

	void Init();
	void Shutdown();

	std::shared_ptr<std::atomic_bool> LoadAsset(task_t&& a_task);

}    // namespace CR::Graphics::AssetLoadingThread
