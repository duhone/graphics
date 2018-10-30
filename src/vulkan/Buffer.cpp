#include "Graphics/Buffer.h"
#include "EngineInternal.h"
#include "vulkan/vulkan.hpp"

using namespace std;
using namespace CR::Graphics;

namespace {
	class VKBuffer : public Buffer {
	  public:
		VKBuffer(BufferType a_type, uint32_t a_bytes);
		VKBuffer(VKBuffer&) = delete;
		VKBuffer& operator=(VKBuffer&) = delete;
		virtual ~VKBuffer();

		void* GetHandle() override { return &m_Buffer; }
		void* Map() override;
		void UnMap() override;

		BufferType m_type;
		vk::Buffer m_Buffer;
		vk::Buffer m_StagingBuffer;
		vk::DeviceMemory m_BufferMemory;
		vk::DeviceMemory m_StagingBufferMemory;
	};
}    // namespace

VKBuffer::VKBuffer(BufferType a_type, uint32_t a_bytes) : m_type(a_type) {
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

VKBuffer::~VKBuffer() {
	GetDevice().destroyBuffer(m_Buffer);
	GetDevice().freeMemory(m_BufferMemory);
	GetDevice().destroyBuffer(m_StagingBuffer);
	GetDevice().freeMemory(m_StagingBufferMemory);
}

void* VKBuffer::Map() {
	return GetDevice().mapMemory(m_StagingBufferMemory, 0, VK_WHOLE_SIZE);
}

void VKBuffer::UnMap() {
	GetDevice().unmapMemory(m_StagingBufferMemory);
}

std::unique_ptr<Buffer> CR::Graphics::CreateBuffer(BufferType a_type, uint32_t a_bytes) {
	return make_unique<VKBuffer>(a_type, a_bytes);
}
