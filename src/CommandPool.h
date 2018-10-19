#pragma once
#include <memory>

namespace CR::Graphics {
	struct CommandBuffer {
		CommandBuffer()                     = default;
		virtual ~CommandBuffer()            = default;
		CommandBuffer(const CommandBuffer&) = delete;
		CommandBuffer& operator=(const CommandBuffer&) = delete;

		virtual void Reset() = 0;
	};

	struct CommandPool {
		// Primary and secondary will be on the graphics queue. Transfer will be on the transfer queue.
		// Primary is optimized for use once command buffers. secondary and transfer is optimized for reusable commands.
		enum class PoolType { Primary, Secondary, Transfer };
		CommandPool()                   = default;
		virtual ~CommandPool()          = default;
		CommandPool(const CommandPool&) = delete;
		CommandPool& operator=(const CommandPool&) = delete;

		virtual std::unique_ptr<CommandBuffer> CreateCommandBuffer() = 0;
	};

	std::unique_ptr<CommandPool> CreateCommandBuffer(CommandPool::PoolType a_type);
}    // namespace CR::Graphics
