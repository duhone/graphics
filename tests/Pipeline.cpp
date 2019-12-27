#include "catch.hpp"

#include "Graphics/Engine.h"
#include "Pipeline.h"
#include "TestFixture.h"
#include <vector>

using namespace CR::Graphics;
using namespace std;

TEST_CASE_METHOD(TestFixture, "pipeline creation/destruction", "") {
	unique_ptr<Pipeline> pipeline = CreatePipeline();
}
