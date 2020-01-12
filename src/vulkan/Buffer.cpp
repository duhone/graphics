#include "Buffer.h"

using namespace std;
using namespace CR::Graphics;

Buffer::Buffer(BufferType a_type, uint32_t a_bytes) : m_type(a_type) {
	assert(a_bytes % 256 == 0);
	vk::BufferCreateInfo createInfo;
	createInfo.flags       = vk::BufferCreateFlags{};
	createInfo.sharingMode = vk::SharingMode::eExclusive;
	createInfo.size        = a_bytes;
	switch(a_type) {
		case BufferType::Index:
			createInfo.usage = vk::BufferUsageFlagBits::eIndexBuffer;
			break;
		case BufferType::Vertex:
			createInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer;
			break;
		case BufferType::Uniform:
			createInfo.usage = vk::BufferUsageFlagBits::eUniformBuffer;
			break;
		default:
			assert(false);
			break;
	}
	createInfo.usage |= vk::BufferUsageFlagBits::eTransferDst;

	// main buffer
	m_Buffer                = GetDevice().createBuffer(createInfo);
	auto bufferRequirements = GetDevice().getBufferMemoryRequirements(m_Buffer);

	vk::MemoryAllocateInfo allocInfo;
	allocInfo.memoryTypeIndex = GetDeviceMemoryIndex();
	assert(bufferRequirements.alignment <= 256);
	allocInfo.allocationSize = bufferRequirements.size;
	m_BufferMemory           = GetDevice().allocateMemory(allocInfo);

	GetDevice().bindBufferMemory(m_Buffer, m_BufferMemory, 0);

	// staging buffer, slow on mobile platforms, but fast way for mobile, wont work on desktop.
	createInfo.usage &= vk::BufferUsageFlagBits::eTransferDst;
	createInfo.usage |= vk::BufferUsageFlagBits::eTransferSrc;
	m_StagingBuffer    = GetDevice().createBuffer(createInfo);
	bufferRequirements = GetDevice().getBufferMemoryRequirements(m_StagingBuffer);

	allocInfo.memoryTypeIndex = GetHostMemoryIndex();
	assert(bufferRequirements.alignment <= 256);
	allocInfo.allocationSize = bufferRequirements.size;
	m_StagingBufferMemory    = GetDevice().allocateMemory(allocInfo);

	GetDevice().bindBufferMemory(m_StagingBuffer, m_StagingBufferMemory, 0);
}

Buffer::~Buffer() {
	Free();
}

Buffer::Buffer(Buffer&& a_other) {
	*this = move(a_other);
}

Buffer& Buffer::operator=(Buffer&& a_other) {
	Free();
	m_Buffer              = a_other.m_Buffer;
	m_BufferMemory        = a_other.m_BufferMemory;
	m_StagingBuffer       = a_other.m_StagingBuffer;
	m_StagingBufferMemory = a_other.m_StagingBufferMemory;
	m_type                = a_other.m_type;

	a_other.m_Buffer              = vk::Buffer{};
	a_other.m_BufferMemory        = vk::DeviceMemory{};
	a_other.m_StagingBuffer       = vk::Buffer{};
	a_other.m_StagingBufferMemory = vk::DeviceMemory{};

	return *this;
}

void Buffer::Free() {
	if(m_Buffer) {
		GetDevice().destroyBuffer(m_Buffer);
		GetDevice().freeMemory(m_BufferMemory);
	}
	if(m_StagingBuffer) {
		GetDevice().destroyBuffer(m_StagingBuffer);
		GetDevice().freeMemory(m_StagingBufferMemory);
	}
}

std::byte* Buffer::Map() {
	return (std::byte*)GetDevice().mapMemory(m_StagingBufferMemory, 0, VK_WHOLE_SIZE);
}

void Buffer::UnMap() {
	GetDevice().unmapMemory(m_StagingBufferMemory);
}
