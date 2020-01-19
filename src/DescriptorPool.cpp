#include "DescriptorPool.h"

#include "vulkan/EngineInternal.h"

using namespace CR;

namespace {
	vk::DescriptorPool m_pool;
}    // namespace

void Graphics::DescriptorPoolInit() {
	vk::DescriptorPoolSize poolSize;
	poolSize.type            = vk::DescriptorType::eUniformBufferDynamic;
	poolSize.descriptorCount = 4;

	vk::DescriptorPoolCreateInfo poolInfo;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes    = &poolSize;
	poolInfo.maxSets       = 1;

	m_pool = GetDevice().createDescriptorPool(poolInfo);
}

void Graphics::DescriptorPoolDestroy() {
	GetDevice().destroyDescriptorPool(m_pool);
}

vk::DescriptorSet Graphics::CreateDescriptorSet(const vk::DescriptorSetLayout& a_layout,
                                                UniformBufferDynamic& a_buffer) {
	vk::DescriptorSetAllocateInfo info;
	info.descriptorPool     = m_pool;
	info.descriptorSetCount = 1;
	info.pSetLayouts        = &a_layout;

	vk::DescriptorSet result = GetDevice().allocateDescriptorSets(info)[0];

	vk::DescriptorBufferInfo bufInfo;
	bufInfo.buffer = a_buffer.GetHandle();
	bufInfo.offset = 0;
	bufInfo.range  = 256 * 8 * 4;

	vk::WriteDescriptorSet writeSet;
	writeSet.dstSet          = result;
	writeSet.dstBinding      = 0;
	writeSet.dstArrayElement = 0;
	writeSet.descriptorType  = vk::DescriptorType::eUniformBufferDynamic;
	writeSet.descriptorCount = 1;
	writeSet.pBufferInfo     = &bufInfo;

	GetDevice().updateDescriptorSets(1, &writeSet, 0, nullptr);

	return result;
}
