#include "ErrorTranslation.h"

std::string_view CR::Graphics::TranslateError(VkResult result) {
	switch(result) {
	case VK_SUCCESS:
		return "VK_SUCCESS!";
		break;
	case VK_NOT_READY:
		return "VK_NOT_READY";
		break;
	case VK_TIMEOUT:
		return "VK_TIMEOUT";
		break;
	case VK_EVENT_SET:
		return "VK_EVENT_SET";
		break;
	case VK_EVENT_RESET:
		return "VK_EVENT_RESET";
		break;
	case VK_INCOMPLETE:
	//case VK_RESULT_END_RANGE:
		return "VK_INCOMPLETE or VK_RESULT_END_RANGE";
		break;
	case VK_ERROR_OUT_OF_HOST_MEMORY:
		return "VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
	case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
	case VK_ERROR_INITIALIZATION_FAILED:
		return "VK_ERROR_INITIALIZATION_FAILED";
		break;
	case VK_ERROR_DEVICE_LOST:
		return "VK_ERROR_DEVICE_LOST";
		break;
	case VK_ERROR_MEMORY_MAP_FAILED:
		return "VK_ERROR_MEMORY_MAP_FAILED";
		break;
	case VK_ERROR_LAYER_NOT_PRESENT:
		return "VK_ERROR_LAYER_NOT_PRESENT";
		break;
	case VK_ERROR_EXTENSION_NOT_PRESENT:
		return "VK_ERROR_EXTENSION_NOT_PRESENT";
		break;
	case VK_ERROR_FEATURE_NOT_PRESENT:
		return "VK_ERROR_FEATURE_NOT_PRESENT";
		break;
	case VK_ERROR_INCOMPATIBLE_DRIVER:
		return "VK_ERROR_INCOMPATIBLE_DRIVER";
		break;
	case VK_ERROR_TOO_MANY_OBJECTS:
		return "VK_ERROR_TOO_MANY_OBJECTS";
		break;
	case VK_ERROR_FORMAT_NOT_SUPPORTED:
		return "VK_ERROR_FORMAT_NOT_SUPPORTED";
		break;
	case VK_ERROR_FRAGMENTED_POOL:
	//case VK_RESULT_BEGIN_RANGE:
		return "VK_ERROR_FRAGMENTED_POOL or VK_RESULT_BEGIN_RANGE";
		break;
	case VK_ERROR_SURFACE_LOST_KHR:
		return "VK_ERROR_SURFACE_LOST_KHR";
		break;
	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
		return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
		break;
	case VK_SUBOPTIMAL_KHR:
		return "VK_SUBOPTIMAL_KHR";
		break;
	case VK_ERROR_OUT_OF_DATE_KHR:
		return "VK_ERROR_OUT_OF_DATE_KHR";
		break;
	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
		return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
		break;
	case VK_ERROR_VALIDATION_FAILED_EXT:
		return "VK_ERROR_VALIDATION_FAILED_EXT";
		break;
	case VK_ERROR_INVALID_SHADER_NV:
		return "VK_ERROR_INVALID_SHADER_NV";
		break;
	case VK_ERROR_OUT_OF_POOL_MEMORY_KHR:
		return "VK_ERROR_OUT_OF_POOL_MEMORY_KHR";
		break;
	case VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR:
		return "VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR";
		break;
	case VK_RESULT_RANGE_SIZE:
		return "VK_RESULT_RANGE_SIZE";
		break;
	case VK_RESULT_MAX_ENUM:
		return "VK_RESULT_MAX_ENUM";
		break;
	default:
		return "Unknown vulkan error";
		break;
	}
}