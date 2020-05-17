#pragma once

#include "EngineInternal.h"

#include <3rdParty/function2.h>

#include <future>
#include <memory>

namespace CR::Graphics {
	class CommandBuffer;
}

namespace CR::Graphics::AssetLoadingThread {
	// First function passed to you will acquire a command buffer, the second function will perform the work you have
	// given in the command buffer, and then wait for that work to be 100% completed
	using task_t = fu2::unique_function<void(fu2::unique_function<CommandBuffer&()>, fu2::unique_function<void()>)>;

	void Init();
	void Shutdown();

	std::shared_ptr<std::atomic_bool> LoadAsset(task_t&& a_task);

}    // namespace CR::Graphics::AssetLoadingThread
