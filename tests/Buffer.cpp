#include "catch.hpp"

#include "Graphics/Buffer.h"
#include "Graphics/Engine.h"
#include "core/literals.h"

using namespace CR::Graphics;
using namespace CR::Core::Literals;

TEST_CASE("buffer creation/destruction", "") {
	EngineSettings settings;
	settings.ApplicationName    = "Unit Test";
	settings.ApplicationVersion = 1;
	settings.EnableDebug        = true;

	REQUIRE_NOTHROW(CreateEngine(settings));

	auto buffer = CreateBuffer(BufferType::Uniform, (uint)1_Kb);
	buffer.reset();

	REQUIRE_NOTHROW(ShutdownEngine());
}
