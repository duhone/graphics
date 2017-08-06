#pragma once
#include "math/Types.h"

namespace CR::Graphics {	
	struct EngineSettings {
		std::string ApplicationName;
		uint ApplicationVersion{0};
		bool TrackMemory{false};
		bool EnableDebug{false};
	};

	void CreateEngine(const EngineSettings& a_settings);
	void ShutdownEngine();
}