#include "catch.hpp"

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
	auto crtex = Platform::OpenMMapFile(Platform::GetCurrentProcessPath() / "spencer_walk.crtexd");
	TextureCreateInfo texInfo;
	texInfo.TextureData = Core::Span<byte>{crtex->data(), crtex->size()};
	texInfo.Name        = "spencer_walk";
	TextureSet texSet   = CreateTextureSet({&texInfo, 1});

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

	Sprite::Props props;
	props.Position = glm::vec2{256.0f, 64.0f};
	props.Color    = glm::vec4{1.0f, 1.0f, 0.0f, 1.0f};
	sprite->SetProps(props);

	glm::vec2 step{1.0f, 2.0f};

	constexpr bool loop = true;
	while(loop && !glfwWindowShouldClose(Window)) {
		glfwPollEvents();
		props.Position += step;
		if(props.Position.x > 1280.0f) { step.x = -1.0f; }
		if(props.Position.x < 0.0f) { step.x = 1.0f; }
		if(props.Position.y > 720.0f) { step.y = -2.0f; }
		if(props.Position.y < 0.0f) { step.y = 2.0f; }
		sprite->SetProps(props);
		Frame();
	}
}
