#include "catch.hpp"

#include "Graphics/Engine.h"
#include "Graphics/Sprite.h"
#include "TestFixture.h"

using namespace CR;
using namespace CR::Graphics;
using namespace std;

TEST_CASE_METHOD(TestFixture, "sprites basic", "") {
	auto sprite = CreateSprite();
}
