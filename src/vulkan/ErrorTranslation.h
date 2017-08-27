#pragma once
#include "vulkan/vulkan.h"
#include <string_view>

namespace CR::Graphics {
	//returned string view is guaranteed to be null terminated.
	std::string_view TranslateError(VkResult result);
}