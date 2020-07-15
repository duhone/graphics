#include <3rdParty/doctest.h>
#include <3rdParty/glm.h>

#include "TestFixture.h"

#include "types/SNorm.h"
#include "types/UNorm.h"

#include "Graphics/Engine.h"
#include "VertexBuffer.h"
#include "core/literals.h"

using namespace CR::Graphics;
using namespace CR::Core::Literals;

TEST_CASE_FIXTURE(TestFixture, "vertexbuffer") {
#pragma pack(push)
#pragma pack(1)
	struct Vertex {
		UNorm2<uint8_t> Loc0;
		SNorm3<int16_t> Loc1;
		float Loc2;
		glm::i16vec4 Loc3;
		glm::u32vec2 Loc4;
		glm::vec3 Loc5;
	};
#pragma pack(pop)
	Vertex dummyVert;    // to build layout, no reflection in C++
	VertexBufferLayout layout;
	layout.AddVariable(dummyVert.Loc0);
	layout.AddVariable(dummyVert.Loc1);
	layout.AddVariable(dummyVert.Loc2);
	layout.AddVariable(dummyVert.Loc3);
	layout.AddVariable(dummyVert.Loc4);
	layout.AddVariable(dummyVert.Loc5);

	auto buffer = VertexBuffer<float>((uint32_t)1_Kb * sizeof(float));
	float* data = buffer.GetData();
	REQUIRE(data != nullptr);
	for(uint32_t i = 0; i < 1_Kb; ++i) { data[i] = 1.0f; }
}
