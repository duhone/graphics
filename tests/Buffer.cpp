#include "catch.hpp"

#include "Graphics/Buffer.h"
#include "Graphics/Engine.h"
#include "core/literals.h"

using namespace CR::Graphics;
using namespace CR::Core::Literals;

TEST_CASE("buffer creation/update/destruction", "") {
	EngineSettings settings;
	settings.ApplicationName    = "Unit Test";
	settings.ApplicationVersion = 1;
	settings.EnableDebug        = true;

	REQUIRE_NOTHROW(CreateEngine(settings));

	auto buffer = CreateBuffer(BufferType::Uniform, (uint32_t)1_Kb * sizeof(float));
	float* data = buffer->Map<float>();
	REQUIRE(data != nullptr);
	for(uint32_t i = 0; i < 1_Kb; ++i) { data[i] = 1.0f; }
	buffer->UnMap();
	buffer.reset();

	REQUIRE_NOTHROW(ShutdownEngine());
}
