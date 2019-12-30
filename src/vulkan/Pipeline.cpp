#include "Pipeline.h"

#include "EngineInternal.h"

#include "DataCompression/LosslessCompression.h"
#include "core/Span.h"

#include "vulkan/vulkan.hpp"

using namespace std;
using namespace CR;
using namespace CR::Graphics;

namespace {
	class PipelineImpl : public Pipeline {
	  public:
		PipelineImpl(const CreatePipelineArgs& a_args);
		~PipelineImpl()                   = default;
		PipelineImpl(const PipelineImpl&) = delete;
		PipelineImpl& operator=(const PipelineImpl&) = delete;

	  private:
		vk::UniquePipelineLayout m_pipeLineLayout;
	};
}    // namespace

PipelineImpl::PipelineImpl(const CreatePipelineArgs& a_args) {
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

	// defaults are fine for this one. we dont have a vertex buffer
	vk::PipelineVertexInputStateCreateInfo vertInputInfo;
	vk::PipelineInputAssemblyStateCreateInfo vertAssemblyInfo;
	vertAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleStrip;

	vk::Viewport viewPort;
	viewPort.width    = (float)GetWindowSize().x;
	viewPort.height   = (float)GetWindowSize().y;
	viewPort.minDepth = 0.0f;
	viewPort.maxDepth = 1.0f;

	vk::Rect2D scissor;
	scissor.extent.width  = GetWindowSize().x;
	scissor.extent.height = GetWindowSize().y;

	vk::PipelineViewportStateCreateInfo viewPortInfo;
	viewPortInfo.pViewports    = &viewPort;
	viewPortInfo.viewportCount = 1;
	viewPortInfo.pScissors     = &scissor;
	viewPortInfo.scissorCount  = 1;

	vk::PipelineRasterizationStateCreateInfo rasterInfo;
	rasterInfo.cullMode = vk::CullModeFlagBits::eNone;

	vk::PipelineMultisampleStateCreateInfo multisampleInfo;
	multisampleInfo.alphaToCoverageEnable = true;
	multisampleInfo.rasterizationSamples  = vk::SampleCountFlagBits::e4;

	vk::PipelineColorBlendAttachmentState blendAttachState;
	blendAttachState.blendEnable = false;

	vk::PipelineColorBlendStateCreateInfo blendStateInfo;
	blendStateInfo.pAttachments    = &blendAttachState;
	blendStateInfo.attachmentCount = 1;

	vk::PipelineLayoutCreateInfo layoutInfo;

	m_pipeLineLayout = GetDevice().createPipelineLayoutUnique(layoutInfo);
}

std::unique_ptr<Pipeline> CR::Graphics::CreatePipeline(const CreatePipelineArgs& a_args) {
	return make_unique<PipelineImpl>(a_args);
}