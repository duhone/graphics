#include "catch.hpp"

#include "Graphics/Engine.h"
#include "Graphics/SpriteTemplate.h"
#include "Graphics/SpriteType.h"
#include "Platform/MemoryMappedFile.h"
#include "Platform/PathUtils.h"

#include "TestFixture.h"

using namespace CR;
using namespace CR::Graphics;
using namespace std;

TEST_CASE_METHOD(TestFixture, "sprite_template_basic", "") {
	auto crsm = Platform::OpenMMapFile(Platform::GetCurrentProcessPath() / "simple.crsm");
	SpriteTypeCreateInfo info;
	info.Name         = "sprite type";
	info.ShaderModule = Core::Span<byte>{crsm->data(), crsm->size()};

	auto spriteType = CreateSpriteType(info);

	SpriteTemplateCreateInfo templateInfo;
	templateInfo.Name      = "test template";
	templateInfo.Type      = spriteType;
	templateInfo.FrameSize = {64, 64};
	auto spriteTemplate    = CreateSpriteTemplate(templateInfo);
}
