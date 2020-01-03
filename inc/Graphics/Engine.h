#pragma once

#include "glm/vec4.hpp"

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
	// Designed this way to reduce latency as much as possible.
	// You should call BeginFrame at the begining of your frame.
	// Then perform your game logic, including sampling input.
	// Then call EndFrame on the graphics engine.
	// BeginFrame will probably block for a while, don't include its cost in any perf analysis
	// BeginFrame blocks until all graphics processing is done, and vsync has happened.
	// This style does cost a lot of performance, but it gives the lowest possible latency in exchange.
	void BeginFrame();
	void EndFrame();
	void ShutdownEngine();
}    // namespace CR::Graphics
