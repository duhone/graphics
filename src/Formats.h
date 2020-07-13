#pragma once

#include "VulkanWindows.h"
#include "types/SNorm.h"
#include "types/UNorm.h"

#include "3rdParty/glm.h"

namespace CR::Graphics {
	template<typename T>
	[[nodiscard]] inline vk::Format GetVKFormat(const SNorm<T>&) noexcept {
		if constexpr(sizeof(T) == 1) {
			return vk::Format::eR8Snorm;
		} else {
			return vk::Format::eR16Snorm;
		}
	}

	template<typename T>
	[[nodiscard]] inline vk::Format GetVKFormat(const SNorm2<T>&) noexcept {
		if constexpr(sizeof(T) == 1) {
			return vk::Format::eR8G8Snorm;
		} else {
			return vk::Format::eR16G16Snorm;
		}
	}

	template<typename T>
	[[nodiscard]] inline vk::Format GetVKFormat(const SNorm3<T>&) noexcept {
		if constexpr(sizeof(T) == 1) {
			return vk::Format::eR8G8B8Snorm;
		} else {
			return vk::Format::eR16G16B16Snorm;
		}
	}

	template<typename T>
	[[nodiscard]] inline vk::Format GetVKFormat(const SNorm4<T>&) noexcept {
		if constexpr(sizeof(T) == 1) {
			return vk::Format::eR8G8B8A8Snorm;
		} else {
			return vk::Format::eR16G16B16A16Snorm;
		}
	}

	template<typename T>
	[[nodiscard]] inline vk::Format GetVKFormat(const UNorm<T>&) noexcept {
		if constexpr(sizeof(T) == 1) {
			return vk::Format::eR8Unorm;
		} else {
			return vk::Format::eR16Unorm;
		}
	}

	template<typename T>
	[[nodiscard]] inline vk::Format GetVKFormat(const UNorm2<T>&) noexcept {
		if constexpr(sizeof(T) == 1) {
			return vk::Format::eR8G8Unorm;
		} else {
			return vk::Format::eR16G16Unorm;
		}
	}

	template<typename T>
	[[nodiscard]] inline vk::Format GetVKFormat(const UNorm3<T>&) noexcept {
		if constexpr(sizeof(T) == 1) {
			return vk::Format::eR8G8B8Unorm;
		} else {
			return vk::Format::eR16G16B16Unorm;
		}
	}

	template<typename T>
	[[nodiscard]] inline vk::Format GetVKFormat(const UNorm4<T>&) noexcept {
		if constexpr(sizeof(T) == 1) {
			return vk::Format::eR8G8B8A8Unorm;
		} else {
			return vk::Format::eR16G16B16A16Unorm;
		}
	}

	[[nodiscard]] inline vk::Format GetVKFormat(const float&) noexcept { return vk::Format::eR32Sfloat; }

	[[nodiscard]] inline vk::Format GetVKFormat(const glm::vec2&) noexcept { return vk::Format::eR32G32Sfloat; }

	[[nodiscard]] inline vk::Format GetVKFormat(const glm::vec3&) noexcept { return vk::Format::eR32G32B32Sfloat; }

	[[nodiscard]] inline vk::Format GetVKFormat(const glm::vec4&) noexcept { return vk::Format::eR32G32B32A32Sfloat; }

	[[nodiscard]] inline vk::Format GetVKFormat(const glm::i8&) noexcept { return vk::Format::eR8Sint; }

	[[nodiscard]] inline vk::Format GetVKFormat(const glm::i8vec2&) noexcept { return vk::Format::eR8G8Sint; }

	[[nodiscard]] inline vk::Format GetVKFormat(const glm::i8vec3&) noexcept { return vk::Format::eR8G8B8Sint; }

	[[nodiscard]] inline vk::Format GetVKFormat(const glm::i8vec4&) noexcept { return vk::Format::eR8G8B8A8Sint; }

	[[nodiscard]] inline vk::Format GetVKFormat(const glm::i16&) noexcept { return vk::Format::eR16Sint; }

	[[nodiscard]] inline vk::Format GetVKFormat(const glm::i16vec2&) noexcept { return vk::Format::eR16G16Sint; }

	[[nodiscard]] inline vk::Format GetVKFormat(const glm::i16vec3&) noexcept { return vk::Format::eR16G16B16Sint; }

	[[nodiscard]] inline vk::Format GetVKFormat(const glm::i16vec4&) noexcept { return vk::Format::eR16G16B16A16Sint; }

	[[nodiscard]] inline vk::Format GetVKFormat(const glm::i32&) noexcept { return vk::Format::eR32Sint; }

	[[nodiscard]] inline vk::Format GetVKFormat(const glm::i32vec2&) noexcept { return vk::Format::eR32G32Sint; }

	[[nodiscard]] inline vk::Format GetVKFormat(const glm::i32vec3&) noexcept { return vk::Format::eR32G32B32Sint; }

	[[nodiscard]] inline vk::Format GetVKFormat(const glm::i32vec4&) noexcept { return vk::Format::eR32G32B32A32Sint; }

	[[nodiscard]] inline vk::Format GetVKFormat(const glm::u8&) noexcept { return vk::Format::eR8Uint; }

	[[nodiscard]] inline vk::Format GetVKFormat(const glm::u8vec2&) noexcept { return vk::Format::eR8G8Uint; }

	[[nodiscard]] inline vk::Format GetVKFormat(const glm::u8vec3&) noexcept { return vk::Format::eR8G8B8Uint; }

	[[nodiscard]] inline vk::Format GetVKFormat(const glm::u8vec4&) noexcept { return vk::Format::eR8G8B8A8Uint; }

	[[nodiscard]] inline vk::Format GetVKFormat(const glm::u16&) noexcept { return vk::Format::eR16Uint; }

	[[nodiscard]] inline vk::Format GetVKFormat(const glm::u16vec2&) noexcept { return vk::Format::eR16G16Uint; }

	[[nodiscard]] inline vk::Format GetVKFormat(const glm::u16vec3&) noexcept { return vk::Format::eR16G16B16Uint; }

	[[nodiscard]] inline vk::Format GetVKFormat(const glm::u16vec4&) noexcept { return vk::Format::eR16G16B16A16Uint; }

	[[nodiscard]] inline vk::Format GetVKFormat(const glm::u32&) noexcept { return vk::Format::eR32Uint; }

	[[nodiscard]] inline vk::Format GetVKFormat(const glm::u32vec2&) noexcept { return vk::Format::eR32G32Uint; }

	[[nodiscard]] inline vk::Format GetVKFormat(const glm::u32vec3&) noexcept { return vk::Format::eR32G32B32Uint; }

	[[nodiscard]] inline vk::Format GetVKFormat(const glm::u32vec4&) noexcept { return vk::Format::eR32G32B32A32Uint; }
}    // namespace CR::Graphics
