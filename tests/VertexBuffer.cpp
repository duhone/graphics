#include <3rdParty/doctest.h>

#include "TestFixture.h"

#include "Graphics/Engine.h"
#include "VertexBuffer.h"
#include "core/literals.h"

using namespace CR::Graphics;
using namespace CR::Core::Literals;

TEST_CASE_FIXTURE(TestFixture, "vertexbuffer") {
	auto buffer = VertexBuffer((uint32_t)1_Kb * sizeof(float));
	float* data = buffer.GetData<float>();
	REQUIRE(data != nullptr);
	for(uint32_t i = 0; i < 1_Kb; ++i) { data[i] = 1.0f; }
}
