#pragma once

#include "EngineInternal.h"

#include <memory>

namespace CR::Graphics {
	class CommandBuffer {
	  public:
		CommandBuffer() = default;
		CommandBuffer(vk::CommandPool& commandPool, const vk::CommandBuffer& buffer);
		~CommandBuffer();
		CommandBuffer(const CommandBuffer&) = delete;
		CommandBuffer(CommandBuffer&& a_other) noexcept;
		CommandBuffer& operator=(const CommandBuffer&) = delete;
		CommandBuffer& operator                        =(CommandBuffer&& a_other) noexcept;

		[[nodiscard]] vk::CommandBuffer& GetHandle();
		void Begin();
		void End();
		void Reset();

	  private:
		vk::CommandPool* m_CommandPool = nullptr;
		vk::CommandBuffer m_Buffer;
	};

	class CommandPool {
	  public:
		// Primary and secondary will be on the graphics queue. Transfer will be on the transfer queue.
		// Primary is optimized for use once command buffers. secondary and transfer is optimized for reusable commands.
		enum class PoolType { Primary, Secondary, Transfer, Invalid };

		CommandPool() = default;
		CommandPool(CommandPool::PoolType a_type);
		~CommandPool();
		CommandPool(const CommandPool&) = delete;
		CommandPool(CommandPool&& a_other) noexcept;
		CommandPool& operator=(const CommandPool&) = delete;
		CommandPool& operator                      =(CommandPool&& a_other) noexcept;

		// If the command pool is destroyed, any command buffers returned here will no longer work.
		[[nodiscard]] CommandBuffer CreateCommandBuffer();

	  private:
		PoolType m_Type = PoolType::Invalid;
		vk::CommandPool m_CommandPool;
	};

	inline CommandBuffer::CommandBuffer(CommandBuffer&& a_other) noexcept { *this = std::move(a_other); }
	inline vk::CommandBuffer& CommandBuffer::GetHandle() { return m_Buffer; }

	inline CommandPool::CommandPool(CommandPool&& a_other) noexcept { *this = std::move(a_other); }
}    // namespace CR::Graphics
