#include "catch.hpp"

#include "Graphics/Engine.h"
#include "Pipeline.h"
#include "Platform/MemoryMappedFile.h"
#include "Platform/PathUtils.h"
#include "TestFixture.h"
#include <vector>

using namespace CR;
using namespace CR::Graphics;
using namespace std;

TEST_CASE_METHOD(TestFixture, "pipeline creation/destruction", "") {
	auto crsm = Platform::OpenMMapFile(Platform::GetCurrentProcessPath() / "simple.crsm");
	Graphics::CreatePipelineArgs pipeArgs;
	pipeArgs.ShaderModule = Core::Span<byte>{crsm->data(), crsm->size()};

	unique_ptr<Pipeline> pipeline = CreatePipeline(pipeArgs);
}
