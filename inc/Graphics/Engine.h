#pragma once
#include <string>

namespace CR::Graphics {
	struct EngineSettings {
		std::string ApplicationName;
		uint32_t ApplicationVersion{0};
		bool EnableDebug{false};
	};

	void CreateEngine(const EngineSettings& a_settings);
	void ShutdownEngine();
}    // namespace CR::Graphics
