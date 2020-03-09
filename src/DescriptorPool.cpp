#include "DescriptorPool.h"

#include "Constants.h"
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
	poolSize[1].descriptorCount = c_maxTextures;

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

void Graphics::UpdateDescriptorSet(const vk::DescriptorSet& a_set, const vk::Sampler& a_sampler,
                                   const CR::Core::Span<vk::ImageView> a_imageViews,
                                   const CR::Core::Span<uint16_t> a_textureIndices) {
	assert(a_imageViews.size() == a_textureIndices.size());

	auto& device = GetDevice();

	std::vector<vk::WriteDescriptorSet> writeSets;
	std::vector<vk::DescriptorImageInfo> imgInfos;
	writeSets.reserve(a_imageViews.size());
	imgInfos.reserve(a_imageViews.size());

	for(uint32_t i = 0; i < a_imageViews.size(); ++i) {
		vk::DescriptorImageInfo& imgInfo = imgInfos.emplace_back();
		imgInfo.imageLayout              = vk::ImageLayout::eShaderReadOnlyOptimal;
		imgInfo.imageView                = a_imageViews[i];
		imgInfo.sampler                  = a_sampler;

		vk::WriteDescriptorSet& writeSet = writeSets.emplace_back();
		writeSet.dstSet                  = a_set;
		writeSet.dstBinding              = 1;
		writeSet.dstArrayElement         = a_textureIndices[i];
		writeSet.descriptorType          = vk::DescriptorType::eCombinedImageSampler;
		writeSet.descriptorCount         = 1;
		writeSet.pImageInfo              = &imgInfo;
	}

	device.updateDescriptorSets((uint32_t)writeSets.size(), writeSets.data(), 0, nullptr);
}
