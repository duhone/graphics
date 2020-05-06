#include <3rdParty/doctest.h>

#include "Graphics/Engine.h"
#include "Graphics/SpriteTemplate.h"
#include "Graphics/SpriteType.h"
#include "Platform/MemoryMappedFile.h"
#include "Platform/PathUtils.h"

#include "TestFixture.h"

using namespace CR;
using namespace CR::Graphics;
using namespace std;

TEST_CASE_FIXTURE(TestFixture, "sprite_template_basic") {
	auto spriteType = CreateSpriteType();

	SpriteTemplateCreateInfo templateInfo;
	templateInfo.Name      = "test template";
	templateInfo.Type      = spriteType;
	templateInfo.FrameSize = {64, 64};
	auto spriteTemplate    = CreateSpriteTemplate(templateInfo);
}
