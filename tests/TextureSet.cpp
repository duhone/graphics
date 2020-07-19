#include <3rdParty/doctest.h>

#include "Graphics/Engine.h"
#include "Graphics/TextureSet.h"
#include "Platform/MemoryMappedFile.h"
#include "Platform/PathUtils.h"
#include "TestFixture.h"
#include <vector>

using namespace CR;
using namespace CR::Graphics;
using namespace std;

TEST_CASE("texture_set") {
	Platform::MemoryMappedFile crtexSpencer(Platform::GetCurrentProcessPath() / "BonusHarrySelect.crtexd");
	Platform::MemoryMappedFile crtexComp(Platform::GetCurrentProcessPath() / "CompletionScreen.crtexd");
	TextureCreateInfo texInfo[2];
	texInfo[0].TextureData = Core::Span<const byte>{crtexSpencer.data(), crtexSpencer.size()};
	texInfo[0].Name        = "spencer_walk";
	texInfo[1].TextureData = Core::Span<const byte>{crtexComp.data(), crtexComp.size()};
	texInfo[1].Name        = "completion_screen";
	TextureSet texSet({texInfo, 2});
}
