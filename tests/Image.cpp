#include "catch.hpp"

#include "Graphics/Engine.h"
#include "Graphics/Image.h"

#include <vector>

using namespace CR::Graphics;
using namespace std;

TEST_CASE("image creation/destruction", "") {
	vector<uint8_t> imageData;
	for(int y = 0; y < 256; ++y) {
		for(int x = 0; x < 256; ++x) {
			imageData.push_back(255);
			imageData.push_back(255);
			imageData.push_back(255);
			imageData.push_back(255);
		}
	}

	EngineSettings settings;
	settings.ApplicationName    = "Unit Test";
	settings.ApplicationVersion = 1;
	settings.EnableDebug        = true;

	REQUIRE_NOTHROW(CreateEngine(settings));

	auto image = CreateImage(ImageType::sRGBAUncompressed, 256, 256, 1, 1);
	image->UpdateImage(move(imageData), 0);
	image.reset();

	REQUIRE_NOTHROW(ShutdownEngine());
}
