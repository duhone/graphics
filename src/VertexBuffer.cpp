#include "VertexBuffer.h"

#include "core/Log.h"

using namespace std;
using namespace CR;
using namespace CR::Graphics;

detail::VertexBufferBase::VertexBufferBase(uint32_t a_bytes, void** a_data) {
	Core::Log::Assert(a_bytes % 256 == 0, "vertex buffers must be a multiple of 256 bytes in size");

	vk::BufferCreateInfo createInfo;
	createInfo.flags       = vk::BufferCreateFlags{};
	createInfo.sharingMode = vk::SharingMode::eExclusive;
	createInfo.size        = a_bytes;
	createInfo.usage       = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst;

	// main buffer
	auto& device            = GetDevice();
	m_buffer                = device.createBuffer(createInfo);
	auto bufferRequirements = device.getBufferMemoryRequirements(m_buffer);

	vk::MemoryAllocateInfo allocInfo;
	allocInfo.memoryTypeIndex = GetDeviceMemoryIndex();
	Core::Log::Assert(bufferRequirements.alignment <= 256,
	                  "Currently assuming a 256 alignment will always be sufficient for uniform buffers");
	allocInfo.allocationSize = bufferRequirements.size;
	m_bufferMemory           = device.allocateMemory(allocInfo);

	device.bindBufferMemory(m_buffer, m_bufferMemory, 0);

	// staging buffer
	createInfo.usage   = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferSrc;
	m_stagingBuffer    = device.createBuffer(createInfo);
	bufferRequirements = device.getBufferMemoryRequirements(m_stagingBuffer);

	vk::MemoryAllocateInfo stagAllocInfo;
	stagAllocInfo.memoryTypeIndex = GetHostMemoryIndex();
	Core::Log::Assert(bufferRequirements.alignment <= 256,
	                  "Currently assuming a 256 alignment will always be sufficient for uniform buffers");
	stagAllocInfo.allocationSize = bufferRequirements.size;
	m_stagingBufferMemory        = device.allocateMemory(stagAllocInfo);

	device.bindBufferMemory(m_stagingBuffer, m_stagingBufferMemory, 0);

	*a_data = device.mapMemory(m_stagingBufferMemory, 0, VK_WHOLE_SIZE);
}

detail::VertexBufferBase::~VertexBufferBase() {
	if(m_buffer) {
		auto& device = GetDevice();
		device.unmapMemory(m_stagingBufferMemory);
		device.destroyBuffer(m_stagingBuffer);
		device.freeMemory(m_stagingBufferMemory);
		device.destroyBuffer(m_buffer);
		device.freeMemory(m_bufferMemory);
	}
}

detail::VertexBufferBase::VertexBufferBase(VertexBufferBase&& a_other) noexcept {
	*this = move(a_other);
}

detail::VertexBufferBase& detail::VertexBufferBase::operator=(VertexBufferBase&& a_other) noexcept {
	this->~VertexBufferBase();

	m_buffer              = a_other.m_buffer;
	m_bufferMemory        = a_other.m_bufferMemory;
	m_stagingBuffer       = a_other.m_stagingBuffer;
	m_stagingBufferMemory = a_other.m_stagingBufferMemory;

	a_other.m_buffer              = vk::Buffer{};
	a_other.m_bufferMemory        = vk::DeviceMemory{};
	a_other.m_stagingBuffer       = vk::Buffer{};
	a_other.m_stagingBufferMemory = vk::DeviceMemory{};

	return *this;
}
