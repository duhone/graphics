#include <3rdParty/doctest.h>

#include "Graphics/Engine.h"
#include "Graphics/Sprite.h"
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

	Platform::MemoryMappedFile crsm(Platform::GetCurrentProcessPath() / "simple.crsm");
	SpriteTypeCreateInfo info;
	info.Name         = "sprite type";
	info.ShaderModule = Core::Span<const byte>{crsm.data(), crsm.size()};

	auto spriteType = CreateSpriteType(info);

	SpriteTemplateCreateInfo templateInfoComp;
	templateInfoComp.Name      = "comp template";
	templateInfoComp.Type      = spriteType;
	templateInfoComp.FrameSize = {480, 320};
	auto spriteTemplateComp    = CreateSpriteTemplate(templateInfoComp);

	SpriteTemplateCreateInfo templateInfoHarry;
	templateInfoHarry.Name      = "harry template";
	templateInfoHarry.Type      = spriteType;
	templateInfoHarry.FrameSize = {186, 291};
	auto spriteTemplateHarry    = CreateSpriteTemplate(templateInfoHarry);

	Graphics::SpriteCreateInfo spriteInfo;
	spriteInfo.Name        = "test sprite1";
	spriteInfo.Template    = spriteTemplateComp;
	spriteInfo.TextureName = "completion_screen";
	auto sprite1           = CreateSprite(spriteInfo);
	spriteInfo.Name        = "test sprite2";
	spriteInfo.Template    = spriteTemplateHarry;
	spriteInfo.TextureName = "bonus_harry";
	auto sprite2           = CreateSprite(spriteInfo);

	Sprite::Props props;
	props.Position = glm::vec2{128.0f, 128.0f};
	props.Color    = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f};
	sprite1->SetProps(props);
	props.Position  = glm::vec2{64.0f, 64.0f};
	props.Color     = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f};
	props.FrameRate = Sprite::eFrameRate::FPS20;
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
