#include "catch.hpp"

#include "Graphics/Engine.h"
#include "Graphics/Image.h"

using namespace CR::Graphics;

TEST_CASE("image creation/destruction", "") {
	EngineSettings settings;
	settings.ApplicationName    = "Unit Test";
	settings.ApplicationVersion = 1;
	settings.EnableDebug        = true;

	REQUIRE_NOTHROW(CreateEngine(settings));

	auto image = CreateImage(ImageType::sRGBAUncompressed, 256, 256, 1, 1);
	image.reset();

	REQUIRE_NOTHROW(ShutdownEngine());
}
