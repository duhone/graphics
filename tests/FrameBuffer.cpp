#include <3rdParty/doctest.h>

#include "TestFixture.h"

using namespace CR::Graphics;
using namespace std;

TEST_CASE_FIXTURE(TestFixture, "framebuffer creation/destruction") {
	// while(!glfwWindowShouldClose(Window)) {
	glfwPollEvents();
	Frame();
	//}
}
