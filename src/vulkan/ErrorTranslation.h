#pragma once
#include "core/Concepts.h"
#include "vulkan/vulkan.h"
#include <exception>
#include <string_view>

namespace CR::Graphics {
	// returned string view is guaranteed to be null terminated.
	std::string TranslateError(VkResult result, bool throwOnFailure = true);
}    // namespace CR::Graphics