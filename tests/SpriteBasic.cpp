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
	TextureSet texSet      = CreateTextureSet(texInfo);

	SpriteTemplateBasicCreateInfo templateInfoComp;
	templateInfoComp.Name      = "comp template";
	templateInfoComp.FrameSize = {480, 320};
	auto spriteTemplateComp    = CreateSpriteTemplateBasic(templateInfoComp);

	SpriteTemplateBasicCreateInfo templateInfoHarry;
	templateInfoHarry.Name      = "harry template";
	templateInfoHarry.FrameSize = {186, 291};
	auto spriteTemplateHarry    = CreateSpriteTemplateBasic(templateInfoHarry);

	Graphics::SpriteBasicCreateInfo spriteInfo;
	spriteInfo.Name        = "test sprite1";
	spriteInfo.Template    = spriteTemplateComp;
	spriteInfo.TextureName = "completion_screen";
	auto sprite1           = CreateSpriteBasic(spriteInfo);
	spriteInfo.Name        = "test sprite2";
	spriteInfo.Template    = spriteTemplateHarry;
	spriteInfo.TextureName = "bonus_harry";
	auto sprite2           = CreateSpriteBasic(spriteInfo);

	SpriteBasic::Props props;
	props.Position = glm::vec2{128.0f, 128.0f};
	props.Color    = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f};
	sprite1->SetProps(props);
	props.Position  = glm::vec2{64.0f, 64.0f};
	props.Color     = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f};
	props.FrameRate = SpriteBasic::eFrameRate::FPS20;
	sprite2->SetProps(props);

	glm::vec2 step{1.0f, 2.0f};

	constexpr bool loop = true;
	while(loop && !glfwWindowShouldClose(Window)) {
		glfwPollEvents();
		props.Position += step;
		if(props.Position.x > 1280.0f) { step.x = -1.0f; }
		if(props.Position.x < 0.0f) { step.x = 1.0f; }
		if(props.Position.y > 720.0f) { step.y = -2.0f; }
		if(props.Position.y < 0.0f) { step.y = 2.0f; }
		sprite2->SetProps(props);
		Frame();
	}
}
