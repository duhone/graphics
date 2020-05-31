#include <3rdParty/doctest.h>

#include "Graphics/Engine.h"
#include "Graphics/SpriteBasic.h"
#include "Graphics/TextureSet.h"
#include "Platform/MemoryMappedFile.h"
#include "Platform/PathUtils.h"
#include "TestFixture.h"

using namespace CR;
using namespace CR::Graphics;
using namespace std;

TEST_CASE_FIXTURE(TestFixture, "sprites_basic") {
	Platform::MemoryMappedFile crtexHarry(Platform::GetCurrentProcessPath() / "BonusHarrySelect.crtexd");
	Platform::MemoryMappedFile crtexComp = (Platform::GetCurrentProcessPath() / "CompletionScreen.crtexd");
	TextureCreateInfo texInfo[2];
	texInfo[0].TextureData = Core::Span<const byte>{crtexHarry.data(), crtexHarry.size()};
	texInfo[0].Name        = "bonus_harry";
	texInfo[1].TextureData = Core::Span<const byte>{crtexComp.data(), crtexComp.size()};
	texInfo[1].Name        = "completion_screen";
	TextureSet texSet(texInfo);

	SpriteTemplateBasicCreateInfo templateInfoComp;
	templateInfoComp.Name        = "comp template";
	templateInfoComp.FrameSize   = {480, 320};
	templateInfoComp.TextureName = "completion_screen";
	auto spriteTemplateComp      = CreateSpriteTemplateBasic(templateInfoComp);

	SpriteTemplateBasicCreateInfo templateInfoHarry;
	templateInfoHarry.Name        = "harry template";
	templateInfoHarry.FrameSize   = {186, 291};
	templateInfoHarry.TextureName = "bonus_harry";
	templateInfoHarry.FrameRate   = eFrameRate::FPS20;
	auto spriteTemplateHarry      = CreateSpriteTemplateBasic(templateInfoHarry);

	Graphics::SpriteBasicCreateInfo spriteInfo;
	spriteInfo.Name     = "test sprite1";
	spriteInfo.Template = spriteTemplateComp;
	SpriteBasic sprite1(spriteInfo);
	spriteInfo.Name     = "test sprite2";
	spriteInfo.Template = spriteTemplateHarry;
	SpriteBasic sprite2(spriteInfo);

	sprite1.SetPosition({128.0f, 128.0f});
	glm::vec2 position{64.0f, 64.0f};
	sprite2.SetPosition(position);

	glm::vec2 step{1.0f, 2.0f};

	bool loop = false;
	do {
		glfwPollEvents();
		position += step;
		if(position.x > 1280.0f) { step.x = -1.0f; }
		if(position.x < 0.0f) { step.x = 1.0f; }
		if(position.y > 720.0f) { step.y = -2.0f; }
		if(position.y < 0.0f) { step.y = 2.0f; }
		sprite2.SetPosition(position);
		Frame();
	} while(loop && !glfwWindowShouldClose(Window));
}

TEST_CASE_FIXTURE(TestFixture, "sprites_rotation") {
	Platform::MemoryMappedFile crtexLeaf(Platform::GetCurrentProcessPath() / "leaf.crtexd");
	Platform::MemoryMappedFile crtexBrick(Platform::GetCurrentProcessPath() / "brick.crtexd");
	TextureCreateInfo texInfo[2];
	texInfo[0].TextureData = Core::Span<const byte>{crtexLeaf.data(), crtexLeaf.size()};
	texInfo[0].Name        = "leaf";
	texInfo[1].TextureData = Core::Span<const byte>{crtexBrick.data(), crtexBrick.size()};
	texInfo[1].Name        = "brick";
	TextureSet texSet(texInfo);

	SpriteTemplateBasicCreateInfo templateInfoLeaf;
	templateInfoLeaf.Name        = "leaf template";
	templateInfoLeaf.FrameSize   = {88, 88};
	templateInfoLeaf.TextureName = "leaf";
	auto spriteTemplateLeaf      = CreateSpriteTemplateBasic(templateInfoLeaf);

	SpriteTemplateBasicCreateInfo templateInfoBrick;
	templateInfoBrick.Name        = "brick template";
	templateInfoBrick.FrameSize   = {88, 88};
	templateInfoBrick.TextureName = "brick";
	auto spriteTemplateBrick      = CreateSpriteTemplateBasic(templateInfoBrick);

	Graphics::SpriteBasicCreateInfo spriteInfo;
	spriteInfo.Name     = "test leaf";
	spriteInfo.Template = spriteTemplateLeaf;
	SpriteBasic spriteLeaf(spriteInfo);
	spriteInfo.Name     = "test brick";
	spriteInfo.Template = spriteTemplateBrick;
	SpriteBasic spriteBrick(spriteInfo);

	spriteLeaf.SetPosition({128.0f, 128.0f});
	spriteBrick.SetPosition({256.0f, 256.0f});

	spriteLeaf.SetRotation(glm::radians(45.0f));

	float brickStep = 1.0f;
	float brickRot  = 0.0f;

	bool loop = true;
	do {
		brickRot += brickStep * m_frameTime;
		spriteBrick.SetRotation(brickRot);

		glfwPollEvents();
		Frame();
	} while(loop && !glfwWindowShouldClose(Window));
}
