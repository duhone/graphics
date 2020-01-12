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
		~PipelineImpl();
		PipelineImpl(const PipelineImpl&) = delete;
		PipelineImpl& operator=(const PipelineImpl&) = delete;

		const std::uintptr_t GetHandle() const override;

	  private:
		vk::PipelineLayout m_pipeLineLayout;
		vk::Pipeline m_pipeline;
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

	glm::vec2 invScreenSize{1.0f / GetWindowSize().x, 1.0f / GetWindowSize().y};
	vk::SpecializationMapEntry specInfoEntrys[2];
	specInfoEntrys[0].constantID = 0;
	specInfoEntrys[0].offset     = offsetof(glm::vec2, x);
	specInfoEntrys[0].size       = sizeof(float);
	specInfoEntrys[0].constantID = 1;
	specInfoEntrys[0].offset     = offsetof(glm::vec2, y);
	specInfoEntrys[0].size       = sizeof(float);

	vk::SpecializationInfo specInfo;
	specInfo.dataSize      = sizeof(invScreenSize);
	specInfo.pData         = &invScreenSize;
	specInfo.mapEntryCount = (uint32_t)size(specInfoEntrys);
	specInfo.pMapEntries   = specInfoEntrys;

	vk::PipelineShaderStageCreateInfo shaderPipeInfo[2];
	shaderPipeInfo[0].module              = vertModule.get();
	shaderPipeInfo[0].pName               = "main";
	shaderPipeInfo[0].stage               = vk::ShaderStageFlagBits::eVertex;
	shaderPipeInfo[0].pSpecializationInfo = &specInfo;
	shaderPipeInfo[1].module              = fragModule.get();
	shaderPipeInfo[1].pName               = "main";
	shaderPipeInfo[1].stage               = vk::ShaderStageFlagBits::eFragment;

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
	rasterInfo.cullMode         = vk::CullModeFlagBits::eNone;
	rasterInfo.lineWidth        = 1.0f;
	rasterInfo.depthClampEnable = false;

	vk::PipelineMultisampleStateCreateInfo multisampleInfo;
	multisampleInfo.alphaToCoverageEnable = true;
	multisampleInfo.rasterizationSamples  = vk::SampleCountFlagBits::e1;

	vk::PipelineColorBlendAttachmentState blendAttachState;
	blendAttachState.blendEnable    = false;
	blendAttachState.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
	                                  vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

	vk::PipelineColorBlendStateCreateInfo blendStateInfo;
	blendStateInfo.pAttachments    = &blendAttachState;
	blendStateInfo.attachmentCount = 1;

	vk::PipelineLayoutCreateInfo layoutInfo;

	m_pipeLineLayout = GetDevice().createPipelineLayout(layoutInfo);

	vk::GraphicsPipelineCreateInfo pipeInfo;
	pipeInfo.layout              = m_pipeLineLayout;
	pipeInfo.pColorBlendState    = &blendStateInfo;
	pipeInfo.pInputAssemblyState = &vertAssemblyInfo;
	pipeInfo.pMultisampleState   = &multisampleInfo;
	pipeInfo.pRasterizationState = &rasterInfo;
	pipeInfo.pVertexInputState   = &vertInputInfo;
	pipeInfo.pViewportState      = &viewPortInfo;
	pipeInfo.stageCount          = 2;
	pipeInfo.pStages             = shaderPipeInfo;
	pipeInfo.renderPass          = GetRenderPass();

	m_pipeline = GetDevice().createGraphicsPipeline(vk::PipelineCache{}, pipeInfo);
}

PipelineImpl::~PipelineImpl() {
	ExecuteNextFrame([pipeline = m_pipeline, pipeLineLayout = m_pipeLineLayout]() {
		GetDevice().destroyPipeline(pipeline);
		GetDevice().destroyPipelineLayout(pipeLineLayout);
	});
}

const std::uintptr_t PipelineImpl::GetHandle() const {
	return (uintptr_t)&m_pipeline;
}

std::unique_ptr<Pipeline> CR::Graphics::CreatePipeline(const CreatePipelineArgs& a_args) {
	return make_unique<PipelineImpl>(a_args);
}
