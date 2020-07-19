#include "VertexBuffer.h"

#include "Commands.h"

#include "core/Log.h"

using namespace std;
using namespace CR;
using namespace CR::Graphics;

detail::VertexBufferBase::VertexBufferBase(const VertexBufferLayout& a_layout, uint32_t a_vertCount, void** a_data) {
	vk::BufferCreateInfo createInfo;
	createInfo.flags       = vk::BufferCreateFlags{};
	createInfo.sharingMode = vk::SharingMode::eExclusive;
	createInfo.size        = a_layout.GetStride() * a_vertCount;
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

	// Only support instance vertex buffers at the moment. And binding would need to be changed in the pipeline as
	// appropriate, although only ever 1 binding at the moment.
	m_bindingDescription.binding   = 0;
	m_bindingDescription.stride    = a_layout.GetStride();
	m_bindingDescription.inputRate = vk::VertexInputRate::eInstance;

	for(const auto& entry : a_layout.m_layout) {
		vk::VertexInputAttributeDescription desc;
		desc.binding  = 0;
		desc.location = entry.Location;
		desc.offset   = entry.Offset;
		desc.format   = entry.format;
		m_attrDescriptions.push_back(desc);
	}
}

detail::VertexBufferBase::~VertexBufferBase() {
	Free();
}

detail::VertexBufferBase::VertexBufferBase(VertexBufferBase&& a_other) noexcept {
	*this = move(a_other);
}

detail::VertexBufferBase& detail::VertexBufferBase::operator=(VertexBufferBase&& a_other) noexcept {
	Free();

	m_buffer              = a_other.m_buffer;
	m_bufferMemory        = a_other.m_bufferMemory;
	m_stagingBuffer       = a_other.m_stagingBuffer;
	m_stagingBufferMemory = a_other.m_stagingBufferMemory;
	m_bindingDescription  = a_other.m_bindingDescription;
	m_attrDescriptions    = std::move(a_other.m_attrDescriptions);

	a_other.m_buffer              = vk::Buffer{};
	a_other.m_bufferMemory        = vk::DeviceMemory{};
	a_other.m_stagingBuffer       = vk::Buffer{};
	a_other.m_stagingBufferMemory = vk::DeviceMemory{};

	return *this;
}

void detail::VertexBufferBase::Free() {
	if(m_buffer) {
		auto& device = GetDevice();
		device.unmapMemory(m_stagingBufferMemory);
		device.destroyBuffer(m_stagingBuffer);
		device.freeMemory(m_stagingBufferMemory);
		device.destroyBuffer(m_buffer);
		device.freeMemory(m_bufferMemory);
	}
}

void detail::VertexBufferBase::Release(CommandBuffer& a_cmdBuffer, uint32_t a_sizeBytes) {
	Commands::CopyBufferToBuffer(a_cmdBuffer, m_stagingBuffer, m_buffer, a_sizeBytes);
	Commands::SetEvent(a_cmdBuffer, m_copyEvent);
}

void detail::VertexBufferBase::Acquire(CommandBuffer& a_cmdBuffer) {
	Commands::WaitEvent(a_cmdBuffer, m_copyEvent);
}
