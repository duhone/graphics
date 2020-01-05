#include "catch.hpp"

#include "Graphics/Engine.h"
#include "Graphics/SpriteType.h"
#include "Platform/MemoryMappedFile.h"
#include "Platform/PathUtils.h"

#include "TestFixture.h"

using namespace CR;
using namespace CR::Graphics;
using namespace std;

TEST_CASE_METHOD(TestFixture, "sprite_type_basic", "") {
	auto crsm = Platform::OpenMMapFile(Platform::GetCurrentProcessPath() / "simple.crsm");
	SpriteTypeCreateInfo info;
	info.Name         = "sprite type";
	info.TextureSize  = glm::uvec2{256, 256};
	info.ShaderModule = Core::Span<byte>{crsm->data(), crsm->size()};

	auto spriteType = CreateSpriteType(info);
}
