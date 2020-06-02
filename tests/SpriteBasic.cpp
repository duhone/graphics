#include <3rdParty/doctest.h>

#include "Graphics/Engine.h"
#include "Graphics/SpriteBasic.h"
#include "Graphics/TextureSet.h"
#include "Platform/MemoryMappedFile.h"
#include "Platform/PathUtils.h"
#include "TestFixture.h"
#include "core/Random.h"

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

	bool loop = false;
	do {
		brickRot += brickStep * m_frameTime;
		spriteBrick.SetRotation(brickRot);

		glfwPollEvents();
		Frame();
	} while(loop && !glfwWindowShouldClose(Window));
}

TEST_CASE_FIXTURE(TestFixture, "sprites_stress") {
	TextureSet texSet;
	{
		vector<TextureCreateInfo> textureInfos;
		vector<Platform::MemoryMappedFile> textureMaps;
		auto addTexture = [&](const char* name) {
			static filesystem::path pathTemplate = Platform::GetCurrentProcessPath() / "{}.crtexd";
			filesystem::path filePath            = fmt::format(pathTemplate.string(), name);
			Platform::MemoryMappedFile crtex(filePath);
			TextureCreateInfo texInfo;
			texInfo.TextureData = Core::Span<const byte>{crtex.data(), crtex.size()};
			texInfo.Name        = name;
			textureInfos.push_back(texInfo);
			textureMaps.push_back(std::move(crtex));
		};
		addTexture("leaf");
		addTexture("brick");
		addTexture("diamond");
		addTexture("gold");
		addTexture("ice");
		addTexture("m");
		addTexture("question");
		addTexture("wood");
		texSet = TextureSet({textureInfos.data(), textureInfos.size()});
	}

	vector<shared_ptr<Graphics::SpriteTemplateBasic>> templates;
	auto addTemplate = [&](const char* name) {
		SpriteTemplateBasicCreateInfo templateInfoLeaf;
		templateInfoLeaf.Name        = fmt::format("{} template", name);
		templateInfoLeaf.FrameSize   = {88, 88};
		templateInfoLeaf.TextureName = name;
		templates.push_back(CreateSpriteTemplateBasic(templateInfoLeaf));
	};
	addTemplate("leaf");
	addTemplate("brick");
	addTemplate("diamond");
	addTemplate("gold");
	addTemplate("ice");
	addTemplate("m");
	addTemplate("question");
	addTemplate("wood");

	struct SpriteData {
		string Name;
		int32_t TemplateIndex{0};
		glm::vec2 Position;
		glm::vec2 PositionStep;
		float Rotation;
		float RotationStep;
	};

	vector<SpriteData> spriteData;
	for(int32_t i = 0; i < 1024; ++i) {
		SpriteData data;
		data.Name           = std::to_string(Core::Random(0LL, numeric_limits<int64_t>::max()));
		data.TemplateIndex  = Core::Random(0, (int32_t)templates.size() - 1);
		data.Position.x     = Core::Random(0.0f, 1280.0f - 88.0f);
		data.Position.y     = Core::Random(0.0f, 720.0f - 88.0f);
		data.PositionStep.x = Core::Random(-2.0f, 2.0f);
		data.PositionStep.y = Core::Random(-2.0f, 2.0f);
		data.Rotation       = Core::Random(0.0f, glm::radians(360.0f));
		data.RotationStep   = Core::Random(glm::radians(-2.0f), glm::radians(2.0f));
		spriteData.push_back(data);
	}

	vector<SpriteBasic> sprites;
	for(const auto& data : spriteData) {
		Graphics::SpriteBasicCreateInfo spriteInfo;
		spriteInfo.Name     = data.Name;
		spriteInfo.Template = templates[data.TemplateIndex];
		SpriteBasic sprite(spriteInfo);
		sprite.SetPosition(data.Position);
		sprite.SetRotation(data.Rotation);
		sprites.push_back(std::move(sprite));
	}

	templates.clear();

	bool loop = true;
	do {
		for(size_t i = 0; i < sprites.size(); ++i) {
			auto pos  = spriteData[i].Position;
			auto step = spriteData[i].PositionStep;
			pos += step;
			if(pos.x > 1280.0f - 88.0f) { step.x = -fabs(step.x); }
			if(pos.x < 0.0f) { step.x = fabs(step.x); }
			if(pos.y > 720.0f - 88.0f) { step.y = -fabs(step.y); }
			if(pos.y < 0.0f) { step.y = fabs(step.y); }
			spriteData[i].PositionStep = step;
			spriteData[i].Position     = pos;
			sprites[i].SetPosition(pos);

			auto rot = spriteData[i].Rotation;
			rot += spriteData[i].RotationStep;
			rot                    = fmod(rot, glm::radians(360.0f));
			spriteData[i].Rotation = rot;
			sprites[i].SetRotation(rot);
		}
		glfwPollEvents();
		Frame();
	} while(loop && !glfwWindowShouldClose(Window));
}
