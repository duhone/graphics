﻿#include "catch.hpp"

#include "Graphics/Engine.h"
#include "Graphics/Sprite.h"
#include "Graphics/TextureSet.h"
#include "Platform/MemoryMappedFile.h"
#include "Platform/PathUtils.h"
#include "TestFixture.h"

using namespace CR;
using namespace CR::Graphics;
using namespace std;

TEST_CASE_METHOD(TestFixture, "sprites_basic", "") {
	auto crtexHarry = Platform::OpenMMapFile(Platform::GetCurrentProcessPath() / "BonusHarrySelect_0.crtexd");
	auto crtexComp  = Platform::OpenMMapFile(Platform::GetCurrentProcessPath() / "CompletionScreen.crtexd");
	TextureCreateInfo texInfo[2];
	texInfo[0].TextureData = Core::Span<byte>{crtexHarry->data(), crtexHarry->size()};
	texInfo[0].Name        = "bonus_harry";
	texInfo[1].TextureData = Core::Span<byte>{crtexComp->data(), crtexComp->size()};
	texInfo[1].Name        = "completion_screen";
	TextureSet texSet      = CreateTextureSet(texInfo);

	auto crsm = Platform::OpenMMapFile(Platform::GetCurrentProcessPath() / "simple.crsm");
	SpriteTypeCreateInfo info;
	info.Name         = "sprite type";
	info.ShaderModule = Core::Span<byte>{crsm->data(), crsm->size()};

	auto spriteType = CreateSpriteType(info);

	SpriteTemplateCreateInfo templateInfo;
	templateInfo.Name      = "test template";
	templateInfo.Type      = spriteType;
	templateInfo.FrameSize = {256, 256};
	auto spriteTemplate    = CreateSpriteTemplate(templateInfo);

	Graphics::SpriteCreateInfo spriteInfo;
	spriteInfo.Name     = "test sprite1";
	spriteInfo.Template = spriteTemplate;
	auto sprite1        = CreateSprite(spriteInfo);
	spriteInfo.Name     = "test sprite2";
	spriteInfo.Template = spriteTemplate;
	auto sprite2        = CreateSprite(spriteInfo);

	Sprite::Props props;
	props.Position = glm::vec2{64128.0f, 64.0f};
	props.Color    = glm::vec4{1.0f, 1.0f, 0.0f, 1.0f};
	sprite1->SetProps(props);
	props.Position = glm::vec2{128.0f, 128.0f};
	props.Color    = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f};
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
		sprite1->SetProps(props);
		Frame();
	}
}
