#pragma once
#include <functional>
#include <string>

namespace CR::Graphics {
	struct EngineSettings {
		std::string ApplicationName;
		uint32_t ApplicationVersion{0};
		bool EnableDebug{false};

		// Platform specific required extensions for windowing system. change to span in c++20.
		const char** ExtensionsToEnable{nullptr};
		uint32_t ExtensionsToEnableCount{0};

		//Little bit hacky, I don't want to include windows.h. so just pass hinstance and hwnd ad void*'s
		//Headless mode isn't currently supported, you must provide a window.
#ifdef WIN32
		void* HInstance{nullptr};
		void* Hwnd{nullptr};
#endif
	};

	void CreateEngine(const EngineSettings& a_settings);
	void ShutdownEngine();
}    // namespace CR::Graphics
