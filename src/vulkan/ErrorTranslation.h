#pragma once
#include "vulkan/vulkan.h"
#include <string_view>
#include <exception>
#include "core/Concepts.h"

namespace CR::Graphics {
	//returned string view is guaranteed to be null terminated.
	std::string TranslateError(VkResult result, bool throwOnFailure = true);
}