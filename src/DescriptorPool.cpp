#include "DescriptorPool.h"

#include "vulkan/EngineInternal.h"

using namespace CR;

namespace {
	vk::DescriptorPool m_pool;
}    // namespace

void Graphics::DescriptorPoolInit() {
	vk::DescriptorPoolSize poolSize;
	poolSize.type            = vk::DescriptorType::eUniformBufferDynamic;
	poolSize.descriptorCount = 1;

	vk::DescriptorPoolCreateInfo poolInfo;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes    = &poolSize;
	poolInfo.maxSets       = 1;

	m_pool = GetDevice().createDescriptorPool(poolInfo);
}

void Graphics::DescriptorPoolDestroy() {
	GetDevice().destroyDescriptorPool(m_pool);
}
