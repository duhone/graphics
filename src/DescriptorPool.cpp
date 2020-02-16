#include "DescriptorPool.h"

#include "vulkan/EngineInternal.h"

using namespace CR;

namespace {
	vk::DescriptorPool m_pool;
}    // namespace

void Graphics::DescriptorPoolInit() {
	vk::DescriptorPoolSize poolSize[2];
	poolSize[0].type            = vk::DescriptorType::eUniformBufferDynamic;
	poolSize[0].descriptorCount = 1;
	poolSize[1].type            = vk::DescriptorType::eCombinedImageSampler;
	poolSize[1].descriptorCount = 1;

	vk::DescriptorPoolCreateInfo poolInfo;
	poolInfo.poolSizeCount = 2;
	poolInfo.pPoolSizes    = poolSize;
	poolInfo.maxSets       = 1;

	m_pool = GetDevice().createDescriptorPool(poolInfo);
}

void Graphics::DescriptorPoolDestroy() {
	GetDevice().destroyDescriptorPool(m_pool);
}

vk::DescriptorSet Graphics::CreateDescriptorSet(const vk::DescriptorSetLayout& a_layout,
                                                UniformBufferDynamic& a_buffer) {
	vk::DescriptorSet result;
	auto& device = GetDevice();

	vk::DescriptorSetAllocateInfo info;
	info.descriptorPool     = m_pool;
	info.descriptorSetCount = 1;
	info.pSetLayouts        = &a_layout;

	result = device.allocateDescriptorSets(info)[0];

	vk::DescriptorBufferInfo bufInfo;
	bufInfo.buffer = a_buffer.GetHandle();
	bufInfo.offset = 0;
	bufInfo.range  = 256 * 8 * 4;

	// vk::DescriptorImageInfo imgInfo;
	// imgInfo.

	vk::WriteDescriptorSet writeSet;
	writeSet.dstSet          = result;
	writeSet.dstBinding      = 0;
	writeSet.dstArrayElement = 0;
	writeSet.descriptorType  = vk::DescriptorType::eUniformBufferDynamic;
	writeSet.descriptorCount = 1;
	writeSet.pBufferInfo     = &bufInfo;

	device.updateDescriptorSets(1, &writeSet, 0, nullptr);

	return result;
}
