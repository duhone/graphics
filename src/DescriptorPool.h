#pragma once

#include "TextureSets.h"
#include "UniformBufferDynamic.h"

#include "vulkan/vulkan.hpp"

namespace CR::Graphics {
	void DescriptorPoolInit();
	void DescriptorPoolDestroy();

	vk::DescriptorSet CreateDescriptorSet(const vk::DescriptorSetLayout& a_layout, UniformBufferDynamic& a_buffer);
}    // namespace CR::Graphics
