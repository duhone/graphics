#include <3rdParty/doctest.h>

#include "Graphics/Engine.h"
#include "Graphics/SpriteTemplateBasic.h"
#include "Graphics/TextureSet.h"
#include "Platform/MemoryMappedFile.h"
#include "Platform/PathUtils.h"

#include "TestFixture.h"

using namespace CR;
using namespace CR::Graphics;
using namespace std;

TEST_CASE_FIXTURE(TestFixture, "sprite_template_basic") {
	Platform::MemoryMappedFile crtexComp = (Platform::GetCurrentProcessPath() / "CompletionScreen.crtexd");
	TextureCreateInfo texInfo;
	texInfo.TextureData = Core::Span<const byte>{crtexComp.data(), crtexComp.size()};
	texInfo.Name        = "completion_screen";
	TextureSet texSet   = CreateTextureSet({&texInfo, 1});

	SpriteTemplateBasicCreateInfo templateInfo;
	templateInfo.Name        = "test template";
	templateInfo.FrameSize   = {64, 64};
	templateInfo.TextureName = "completion_screen";
	auto spriteTemplate      = CreateSpriteTemplateBasic(templateInfo);
}
