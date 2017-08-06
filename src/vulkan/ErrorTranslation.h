#pragma once
#include "vulkan/vulkan.h"
#include <string_view>

namespace CR::Graphics {
	std::string_view TranslateError(VkResult result);
}