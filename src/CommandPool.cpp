#include "CommandPool.h"

using namespace CR::Graphics;
using namespace std;

CommandPool::CommandPool(CommandPool::PoolType a_type) : m_Type(a_type) {
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

CommandPool::~CommandPool() {
	if(m_Type != PoolType::Invalid) { GetDevice().destroyCommandPool(m_CommandPool); }
	m_Type = PoolType::Invalid;
}

CommandPool& CommandPool::operator=(CommandPool&& a_other) noexcept {
	this->~CommandPool();

	m_Type        = a_other.m_Type;
	m_CommandPool = a_other.m_CommandPool;

	a_other.m_Type = PoolType::Invalid;

	return *this;
}

CommandBuffer CommandPool::CreateCommandBuffer() {
	vk::CommandBufferAllocateInfo info;
	info.commandBufferCount = 1;
	info.commandPool        = m_CommandPool;
	info.level = m_Type == PoolType::Secondary ? vk::CommandBufferLevel::eSecondary : vk::CommandBufferLevel::ePrimary;
	vk::CommandBuffer buffer;
	buffer = GetDevice().allocateCommandBuffers(info)[0];
	return CommandBuffer(m_CommandPool, buffer);
}

CommandBuffer::CommandBuffer(vk::CommandPool& commandPool, const vk::CommandBuffer& buffer) :
    m_CommandPool(&commandPool), m_Buffer(buffer) {}

CommandBuffer::~CommandBuffer() {
	if(m_CommandPool != nullptr) { GetDevice().freeCommandBuffers(*m_CommandPool, 1, &m_Buffer); }
	m_CommandPool = nullptr;
}

CommandBuffer& CommandBuffer::operator=(CommandBuffer&& a_other) noexcept {
	this->~CommandBuffer();
	m_CommandPool = a_other.m_CommandPool;
	m_Buffer      = a_other.m_Buffer;

	a_other.m_CommandPool = nullptr;

	return *this;
}

void CommandBuffer::Begin() {
	vk::CommandBufferBeginInfo info;
	info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
	m_Buffer.begin(info);
}

void CommandBuffer::End() {
	m_Buffer.end();
}

void CommandBuffer::Reset() {
	m_Buffer.reset(vk::CommandBufferResetFlags{});
}
