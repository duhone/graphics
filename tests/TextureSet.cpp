#include "catch.hpp"

#include "Graphics/Engine.h"
#include "Graphics/TextureSet.h"
#include "Platform/MemoryMappedFile.h"
#include "Platform/PathUtils.h"
#include "TestFixture.h"
#include <vector>

using namespace CR;
using namespace CR::Graphics;
using namespace std;

TEST_CASE_METHOD(TestFixture, "texture_set", "") {
	auto crtexSpencer = Platform::OpenMMapFile(Platform::GetCurrentProcessPath() / "BonusHarrySelect.crtexd");
	auto crtexComp    = Platform::OpenMMapFile(Platform::GetCurrentProcessPath() / "CompletionScreen.crtexd");
	TextureCreateInfo texInfo[2];
	texInfo[0].TextureData = Core::Span<byte>{crtexSpencer->data(), crtexSpencer->size()};
	texInfo[0].Name        = "spencer_walk";
	texInfo[1].TextureData = Core::Span<byte>{crtexComp->data(), crtexComp->size()};
	texInfo[1].Name        = "completion_screen";
	TextureSet texSet      = CreateTextureSet({texInfo, 2});
}
