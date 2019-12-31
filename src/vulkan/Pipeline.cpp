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

		const std::uintptr_t GetHandle() const override;

	  private:
		vk::UniquePipelineLayout m_pipeLineLayout;
		vk::UniquePipeline m_pipeline;
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

	vk::PipelineShaderStageCreateInfo shaderPipeInfo[2];
	shaderPipeInfo[0].module = vertModule.get();
	shaderPipeInfo[0].pName  = "main";
	shaderPipeInfo[0].stage  = vk::ShaderStageFlagBits::eVertex;
	shaderPipeInfo[1].module = fragModule.get();
	shaderPipeInfo[1].pName  = "main";
	shaderPipeInfo[1].stage  = vk::ShaderStageFlagBits::eFragment;

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
	rasterInfo.cullMode  = vk::CullModeFlagBits::eNone;
	rasterInfo.lineWidth = 1.0f;

	vk::PipelineMultisampleStateCreateInfo multisampleInfo;
	multisampleInfo.alphaToCoverageEnable = true;
	multisampleInfo.rasterizationSamples  = vk::SampleCountFlagBits::e1;

	vk::PipelineColorBlendAttachmentState blendAttachState;
	blendAttachState.blendEnable = false;

	vk::PipelineColorBlendStateCreateInfo blendStateInfo;
	blendStateInfo.pAttachments    = &blendAttachState;
	blendStateInfo.attachmentCount = 1;

	vk::PipelineLayoutCreateInfo layoutInfo;

	m_pipeLineLayout = GetDevice().createPipelineLayoutUnique(layoutInfo);

	vk::GraphicsPipelineCreateInfo pipeInfo;
	pipeInfo.layout              = m_pipeLineLayout.get();
	pipeInfo.pColorBlendState    = &blendStateInfo;
	pipeInfo.pInputAssemblyState = &vertAssemblyInfo;
	pipeInfo.pMultisampleState   = &multisampleInfo;
	pipeInfo.pRasterizationState = &rasterInfo;
	pipeInfo.pVertexInputState   = &vertInputInfo;
	pipeInfo.pViewportState      = &viewPortInfo;
	pipeInfo.stageCount          = 2;
	pipeInfo.pStages             = shaderPipeInfo;
	pipeInfo.renderPass          = GetRenderPass();

	m_pipeline = GetDevice().createGraphicsPipelineUnique(vk::PipelineCache{}, pipeInfo);
}

const std::uintptr_t PipelineImpl::GetHandle() const {
	return (uintptr_t)&m_pipeline.get();
}

std::unique_ptr<Pipeline> CR::Graphics::CreatePipeline(const CreatePipelineArgs& a_args) {
	return make_unique<PipelineImpl>(a_args);
}