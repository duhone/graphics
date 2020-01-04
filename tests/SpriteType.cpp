#include "catch.hpp"

#include "Graphics/Engine.h"
#include "Graphics/SpriteType.h"
#include "TestFixture.h"

using namespace CR;
using namespace CR::Graphics;
using namespace std;

TEST_CASE_METHOD(TestFixture, "sprite type basic", "") {
	auto spriteType = CreateSpriteType();
}
