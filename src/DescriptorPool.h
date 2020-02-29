#pragma once

#include "TextureSets.h"
#include "UniformBufferDynamic.h"

#include "core/Span.h"

#include "vulkan/vulkan.hpp"

namespace CR::Graphics {
	void DescriptorPoolInit();
	void DescriptorPoolDestroy();

	vk::DescriptorSet CreateDescriptorSet(const vk::DescriptorSetLayout& a_layout, UniformBufferDynamic& a_buffer);
	void UpdateDescriptorSet(const vk::DescriptorSet& a_set, const vk::Sampler& a_sampler,
	                         const CR::Core::Span<vk::ImageView> a_imageViews,
	                         const CR::Core::Span<uint16_t> a_textureIndices);
}    // namespace CR::Graphics
