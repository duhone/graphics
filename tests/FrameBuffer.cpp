#include "catch.hpp"

#include "TestFixture.h"

using namespace CR::Graphics;
using namespace std;

TEST_CASE_METHOD(TestFixture, "framebuffer creation/destruction", "") {
	constexpr bool loop = false;
	while(loop && !glfwWindowShouldClose(Window)) {
		glfwPollEvents();
		BeginFrame();
		EndFrame();
	}
}