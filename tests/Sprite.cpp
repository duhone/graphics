#include "catch.hpp"

#include "Graphics/Engine.h"
#include "Graphics/Sprite.h"
#include "Platform/MemoryMappedFile.h"
#include "Platform/PathUtils.h"
#include "TestFixture.h"

using namespace CR;
using namespace CR::Graphics;
using namespace std;

TEST_CASE_METHOD(TestFixture, "sprites_basic", "") {
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

	Graphics::SpriteCreateInfo spriteInfo;
	spriteInfo.Name     = "test sprite";
	spriteInfo.Template = spriteTemplate;
	auto sprite         = CreateSprite(spriteInfo);

	constexpr bool loop = false;
	while(loop && !glfwWindowShouldClose(Window)) {
		glfwPollEvents();
		Frame();
	}
}
