#pragma once

#include <3rdParty/glm.h>

#include <functional>
#include <optional>
#include <string>

namespace CR::Graphics {
	struct EngineSettings {
		std::string ApplicationName;
		uint32_t ApplicationVersion{0};
		bool EnableDebug{false};
		// only set ClearColor if you actually need to clear the screen
		std::optional<glm::vec4> ClearColor;

		// Platform specific required extensions for windowing system. change to span in c++20.
		const char** ExtensionsToEnable{nullptr};
		uint32_t ExtensionsToEnableCount{0};

		// Little bit hacky, I don't want to include windows.h. so just pass hinstance and hwnd ad void*'s
		// Headless mode isn't currently supported, you must provide a window.
#ifdef WIN32
		void* HInstance{nullptr};
		void* Hwnd{nullptr};
#endif
	};

	void CreateEngine(const EngineSettings& a_settings);
	// This will block until GPU is idle(to minimize latency), so should not be including in a simple wall clock
	// profiler
	void Frame();
	void ShutdownEngine();
}    // namespace CR::Graphics
