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

TEST_CASE("vertexbuffer") {
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

	auto buffer = VertexBuffer<Vertex>(layout, 1000);

	REQUIRE(!buffer.empty());
	for(Vertex& vert : buffer) {
		vert.Loc0 = {0.2f, 0.3f};
		vert.Loc1 = {-0.5f, 0.5f, 0.0f};
		vert.Loc2 = 1.0f;
		vert.Loc3 = {0, 1, 2, 3};
		vert.Loc4 = {4, 5};
		vert.Loc5 = {0.0f, 20.0f, -2000.0f};
	}
}
