#include "catch.hpp"

#include "TestFixture.h"

#include "Buffer.h"
#include "Graphics/Engine.h"
#include "core/literals.h"

using namespace CR::Graphics;
using namespace CR::Core::Literals;

TEST_CASE_METHOD(TestFixture, "buffer creation/update/destruction", "") {
	auto buffer = Buffer(BufferType::Uniform, (uint32_t)1_Kb * sizeof(float));
	float* data = buffer.Map<float>();
	REQUIRE(data != nullptr);
	for(uint32_t i = 0; i < 1_Kb; ++i) { data[i] = 1.0f; }
	buffer.UnMap();
}
