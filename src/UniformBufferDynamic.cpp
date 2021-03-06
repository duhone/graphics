﻿#include "UniformBufferDynamic.h"

#include "core/Log.h"

using namespace std;
using namespace CR;
using namespace CR::Graphics;

// Note that this is implemented in an ideal way for mobile hardware, it is slow for desktop hardware
UniformBufferDynamic::UniformBufferDynamic(uint32_t a_bytes) : m_size(a_bytes) {
	Core::Log::Assert(a_bytes % 256 == 0, "uniform buffers must be a multiple of 256 bytes in size");

	vk::BufferCreateInfo createInfo;
	createInfo.flags       = vk::BufferCreateFlags{};
	createInfo.sharingMode = vk::SharingMode::eExclusive;
	createInfo.size        = a_bytes;
	createInfo.usage       = vk::BufferUsageFlagBits::eUniformBuffer;

	// main buffer
	auto& device            = GetDevice();
	m_Buffer                = device.createBuffer(createInfo);
	auto bufferRequirements = device.getBufferMemoryRequirements(m_Buffer);

	vk::MemoryAllocateInfo allocInfo;
	allocInfo.memoryTypeIndex = GetHostMemoryIndex();
	Core::Log::Assert(bufferRequirements.alignment <= 256,
	                  "Currently assuming a 256 alignment will always be sufficient for uniform buffers");
	allocInfo.allocationSize = bufferRequirements.size;
	m_BufferMemory           = device.allocateMemory(allocInfo);

	device.bindBufferMemory(m_Buffer, m_BufferMemory, 0);

	m_data = (std::byte*)device.mapMemory(m_BufferMemory, 0, VK_WHOLE_SIZE);
}

UniformBufferDynamic::~UniformBufferDynamic() {
	Free();
}

UniformBufferDynamic::UniformBufferDynamic(UniformBufferDynamic&& a_other) noexcept {
	*this = move(a_other);
}

UniformBufferDynamic& UniformBufferDynamic::operator=(UniformBufferDynamic&& a_other) noexcept {
	Free();
	m_Buffer       = a_other.m_Buffer;
	m_BufferMemory = a_other.m_BufferMemory;
	m_data         = a_other.m_data;
	m_size         = a_other.m_size;

	a_other.m_Buffer       = vk::Buffer{};
	a_other.m_BufferMemory = vk::DeviceMemory{};
	a_other.m_data         = nullptr;
	a_other.m_size         = 0;

	return *this;
}

void UniformBufferDynamic::Free() {
	if(m_Buffer) {
		auto& device = GetDevice();
		device.unmapMemory(m_BufferMemory);
		device.destroyBuffer(m_Buffer);
		device.freeMemory(m_BufferMemory);
	}
}
