#include <3rdParty/doctest.h>

#include "Graphics/Engine.h"
#include "Graphics/SpriteType.h"
#include "Platform/MemoryMappedFile.h"
#include "Platform/PathUtils.h"

#include "TestFixture.h"

using namespace CR;
using namespace CR::Graphics;
using namespace std;

TEST_CASE_FIXTURE(TestFixture, "sprite_type_basic") {
	auto spriteType = CreateSpriteType();
}
