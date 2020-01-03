#include "CommandPool.h"
#include "EngineInternal.h"

using namespace CR::Graphics;
using namespace std;

namespace {
	class CommandPoolImpl : public CommandPool {
	  public:
		CommandPoolImpl(CommandPool::PoolType a_type);
		virtual ~CommandPoolImpl();
		CommandPoolImpl(const CommandPoolImpl&) = delete;
		CommandPoolImpl& operator=(const CommandPoolImpl&) = delete;

	  private:
		std::unique_ptr<CommandBuffer> CreateCommandBuffer() override;

		PoolType m_Type;
		vk::CommandPool m_CommandPool;
	};

	class CommandBufferImpl : public CommandBuffer {
	  public:
		CommandBufferImpl(vk::CommandPool& commandPool, const vk::CommandBuffer& buffer);
		virtual ~CommandBufferImpl();
		CommandBufferImpl(const CommandBufferImpl&) = delete;
		CommandBufferImpl& operator=(const CommandBufferImpl&) = delete;

	  private:
		void* GetHandle() override { return &m_Buffer; }
		void Begin() override;
		void End() override;
		void Reset() override;

		vk::CommandPool& m_CommandPool;
		vk::CommandBuffer m_Buffer;
	};
}    // namespace

CommandPoolImpl::CommandPoolImpl(CommandPool::PoolType a_type) : m_Type(a_type) {
	vk::CommandPoolCreateInfo info;
	info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
	switch(m_Type) {
		case CR::Graphics::CommandPool::PoolType::Primary:
			info.queueFamilyIndex = GetGraphicsQueueIndex();
			info.flags |= vk::CommandPoolCreateFlagBits::eTransient;
			break;
		case CR::Graphics::CommandPool::PoolType::Secondary:
			info.queueFamilyIndex = GetGraphicsQueueIndex();
			break;
		case CR::Graphics::CommandPool::PoolType::Transfer:
			info.queueFamilyIndex = GetTransferQueueIndex();
			info.flags |= vk::CommandPoolCreateFlagBits::eTransient;
			break;
		default:
			break;
	}
	m_CommandPool = GetDevice().createCommandPool(info);
}

CommandPoolImpl::~CommandPoolImpl() {
	GetDevice().destroyCommandPool(m_CommandPool);
}

std::unique_ptr<CommandBuffer> CommandPoolImpl::CreateCommandBuffer() {
	vk::CommandBufferAllocateInfo info;
	info.commandBufferCount = 1;
	info.commandPool        = m_CommandPool;
	info.level = m_Type == PoolType::Secondary ? vk::CommandBufferLevel::eSecondary : vk::CommandBufferLevel::ePrimary;
	vk::CommandBuffer buffer = GetDevice().allocateCommandBuffers(info)[0];
	return make_unique<CommandBufferImpl>(m_CommandPool, buffer);
}

CommandBufferImpl::CommandBufferImpl(vk::CommandPool& commandPool, const vk::CommandBuffer& buffer) :
    m_CommandPool(commandPool), m_Buffer(buffer) {}

CommandBufferImpl::~CommandBufferImpl() {
	GetDevice().freeCommandBuffers(m_CommandPool, 1, &m_Buffer);
}

void CommandBufferImpl::Begin() {
	vk::CommandBufferBeginInfo info;
	info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
	m_Buffer.begin(info);
}

void CommandBufferImpl::End() {
	m_Buffer.end();
}

void CommandBufferImpl::Reset() {
	m_Buffer.reset(vk::CommandBufferResetFlags{});
}

std::unique_ptr<CommandPool> CR::Graphics::CreateCommandPool(CommandPool::PoolType a_type) {
	return make_unique<CommandPoolImpl>(a_type);
}
