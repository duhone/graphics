#include <3rdParty/doctest.h>

#include "Graphics/Engine.h"
#include "Graphics/SpriteTemplateBasic.h"
#include "Platform/MemoryMappedFile.h"
#include "Platform/PathUtils.h"

#include "TestFixture.h"

using namespace CR;
using namespace CR::Graphics;
using namespace std;

TEST_CASE_FIXTURE(TestFixture, "sprite_template_basic") {
	SpriteTemplateBasicCreateInfo templateInfo;
	templateInfo.Name      = "test template";
	templateInfo.FrameSize = {64, 64};
	auto spriteTemplate    = CreateSpriteTemplateBasic(templateInfo);
}
