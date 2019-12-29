#include "Pipeline.h"

#include "EngineInternal.h"

#include "DataCompression/LosslessCompression.h"
#include "core/Span.h"

#include "vulkan/vulkan.hpp"

using namespace std;
using namespace CR;
using namespace CR::Graphics;

std::unique_ptr<Pipeline> CR::Graphics::CreatePipeline(const CreatePipelineArgs& a_args) {
	auto crsm = DataCompression::Decompress(a_args.ShaderModule.data(), (uint32_t)a_args.ShaderModule.size());

	struct Header {
		uint32_t VertSize{0};
		uint32_t FragSize{0};
	};
	Header header;
	memcpy(&header, crsm.data(), sizeof(Header));

	Core::Span<byte> vertShader{crsm.data() + sizeof(Header), header.VertSize};
	Core::Span<byte> fragShader{crsm.data() + sizeof(Header) + header.VertSize, header.FragSize};

	vk::ShaderModuleCreateInfo vertInfo;
	vertInfo.pCode    = (uint32_t*)vertShader.data();
	vertInfo.codeSize = vertShader.size();

	vk::ShaderModuleCreateInfo fragInfo;
	fragInfo.pCode    = (uint32_t*)fragShader.data();
	fragInfo.codeSize = fragShader.size();

	vk::UniqueShaderModule vertModule = GetDevice().createShaderModuleUnique(vertInfo);
	vk::UniqueShaderModule fragModule = GetDevice().createShaderModuleUnique(fragInfo);

	vk::PipelineShaderStageCreateInfo vertPipeInfo;
	vertPipeInfo.module = vertModule.get();
	vertPipeInfo.pName  = "main";
	vertPipeInfo.stage  = vk::ShaderStageFlagBits::eVertex;
	vk::PipelineShaderStageCreateInfo fragPipeInfo;
	fragPipeInfo.module = fragModule.get();
	fragPipeInfo.pName  = "main";
	fragPipeInfo.stage  = vk::ShaderStageFlagBits::eFragment;

	return nullptr;
}