#include "Pipeline.h"

#include "Constants.h"
#include "EngineInternal.h"
#include "TextureSets.h"

#include "DataCompression/LosslessCompression.h"
#include "core/BinaryStream.h"
#include "core/Span.h"

using namespace std;
using namespace CR;
using namespace CR::Graphics;

Pipeline::Pipeline(const CreatePipelineArgs& a_args) {
	auto& device = GetDevice();
#pragma pack(1)
	static const uint32_t c_FourCC  = 'CRSM';
	static const uint16_t c_Version = 1;
	struct Header {
		uint32_t FourCC{c_FourCC};
		uint16_t Version{c_Version};
		uint16_t VertSize{0};
		uint16_t FragSize{0};
	};
#pragma pack()
	Header header;
	Core::BinaryReader reader;
	reader.Data   = a_args.ShaderModule.data();
	reader.Offset = 0;
	reader.Size   = (uint32_t)a_args.ShaderModule.size();

	Core::Read(reader, header);
	Core::Log::Require(header.FourCC == c_FourCC, "Shader is not a crsm file");
	Core::Log::Require(header.Version == c_Version, "Shader module is wrong version, must be version 1");

	// header holds the uncompressed size, the compressed size is stored right before the buffer
	uint32_t bufferSize = 0;
	Core::Read(reader, bufferSize);

	auto vertShader = DataCompression::Decompress(Core::Span{reader.Data + reader.Offset, bufferSize});
	reader.Offset += bufferSize;
	Core::Log::Require(header.VertSize == vertShader.size(), "corrupt shader module file");

	Core::Read(reader, bufferSize);
	auto fragShader = DataCompression::Decompress(Core::Span{reader.Data + reader.Offset, bufferSize});
	Core::Log::Require(header.FragSize == fragShader.size(), "corrupt shader module file");

	vk::ShaderModuleCreateInfo vertInfo;
	vertInfo.pCode    = (uint32_t*)vertShader.data();
	vertInfo.codeSize = vertShader.size();

	vk::ShaderModuleCreateInfo fragInfo;
	fragInfo.pCode    = (uint32_t*)fragShader.data();
	fragInfo.codeSize = fragShader.size();

	vk::UniqueShaderModule vertModule = device.createShaderModuleUnique(vertInfo);
	vk::UniqueShaderModule fragModule = device.createShaderModuleUnique(fragInfo);

	glm::vec2 invScreenSize{1.0f / GetWindowSize().x, 1.0f / GetWindowSize().y};
	vk::SpecializationMapEntry vertSpecInfoEntrys[2];
	vertSpecInfoEntrys[0].constantID = 0;
	vertSpecInfoEntrys[0].offset     = offsetof(glm::vec2, x);
	vertSpecInfoEntrys[0].size       = sizeof(float);
	vertSpecInfoEntrys[1].constantID = 1;
	vertSpecInfoEntrys[1].offset     = offsetof(glm::vec2, y);
	vertSpecInfoEntrys[1].size       = sizeof(float);

	vk::SpecializationInfo vertSpecInfo;
	vertSpecInfo.dataSize      = sizeof(invScreenSize);
	vertSpecInfo.pData         = &invScreenSize;
	vertSpecInfo.mapEntryCount = (uint32_t)size(vertSpecInfoEntrys);
	vertSpecInfo.pMapEntries   = vertSpecInfoEntrys;

	vk::SpecializationMapEntry fragSpecInfoEntrys;
	fragSpecInfoEntrys.constantID = 0;
	fragSpecInfoEntrys.offset     = 0;
	fragSpecInfoEntrys.size       = sizeof(int32_t);

	vk::SpecializationInfo fragSpecInfo;
	fragSpecInfo.dataSize      = sizeof(invScreenSize);
	fragSpecInfo.pData         = &c_maxTextures;
	fragSpecInfo.mapEntryCount = 1;
	fragSpecInfo.pMapEntries   = &fragSpecInfoEntrys;

	vk::PipelineShaderStageCreateInfo shaderPipeInfo[2];
	shaderPipeInfo[0].module              = vertModule.get();
	shaderPipeInfo[0].pName               = "main";
	shaderPipeInfo[0].stage               = vk::ShaderStageFlagBits::eVertex;
	shaderPipeInfo[0].pSpecializationInfo = &vertSpecInfo;
	shaderPipeInfo[1].module              = fragModule.get();
	shaderPipeInfo[1].pName               = "main";
	shaderPipeInfo[1].stage               = vk::ShaderStageFlagBits::eFragment;
	shaderPipeInfo[1].pSpecializationInfo = &fragSpecInfo;

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

	vk::PushConstantRange pushRange;
	pushRange.offset     = 0;
	pushRange.size       = sizeof(glm::vec2);
	pushRange.stageFlags = vk::ShaderStageFlagBits::eVertex;

	vk::SamplerCreateInfo samplerInfo;
	samplerInfo.addressModeU     = vk::SamplerAddressMode::eClampToEdge;
	samplerInfo.addressModeV     = vk::SamplerAddressMode::eClampToEdge;
	samplerInfo.addressModeW     = vk::SamplerAddressMode::eClampToEdge;
	samplerInfo.minFilter        = vk::Filter::eLinear;
	samplerInfo.magFilter        = vk::Filter::eLinear;
	samplerInfo.mipmapMode       = vk::SamplerMipmapMode::eNearest;
	samplerInfo.anisotropyEnable = false;

	m_sampler = GetDevice().createSampler(samplerInfo);

	// Have to pass one sampler per descriptor. but only using one sampler, so just have to duplicate
	vector<vk::Sampler> samplers;
	samplers.reserve(c_maxTextures);
	for(int32_t i = 0; i < c_maxTextures; ++i) { samplers.push_back(m_sampler); }

	vk::DescriptorSetLayoutBinding dslBinding[2];
	dslBinding[0].binding            = 0;
	dslBinding[0].descriptorCount    = 1;
	dslBinding[0].descriptorType     = vk::DescriptorType::eUniformBufferDynamic;
	dslBinding[0].stageFlags         = vk::ShaderStageFlagBits::eVertex;
	dslBinding[1].binding            = 1;
	dslBinding[1].descriptorCount    = c_maxTextures;
	dslBinding[1].descriptorType     = vk::DescriptorType::eCombinedImageSampler;
	dslBinding[1].stageFlags         = vk::ShaderStageFlagBits::eFragment;
	dslBinding[1].pImmutableSamplers = samplers.data();

	vk::DescriptorSetLayoutCreateInfo dslInfo;
	dslInfo.bindingCount = 2;
	dslInfo.pBindings    = dslBinding;

	m_descriptorSetLayout = device.createDescriptorSetLayout(dslInfo);

	vk::PipelineLayoutCreateInfo layoutInfo;
	layoutInfo.pushConstantRangeCount = 1;
	layoutInfo.pPushConstantRanges    = &pushRange;
	layoutInfo.setLayoutCount         = 1;
	layoutInfo.pSetLayouts            = &m_descriptorSetLayout;

	m_pipeLineLayout = device.createPipelineLayout(layoutInfo);

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

	m_pipeline = device.createGraphicsPipeline(vk::PipelineCache{}, pipeInfo);
}

Pipeline::Pipeline(Pipeline&& a_other) {
	*this = move(a_other);
}

Pipeline& Pipeline::operator=(Pipeline&& a_other) {
	Free();

	m_pipeline            = a_other.m_pipeline;
	m_pipeLineLayout      = a_other.m_pipeLineLayout;
	m_descriptorSetLayout = a_other.m_descriptorSetLayout;
	m_sampler             = a_other.m_sampler;

	a_other.m_pipeline            = vk::Pipeline{};
	a_other.m_pipeLineLayout      = vk::PipelineLayout{};
	a_other.m_descriptorSetLayout = vk::DescriptorSetLayout{};
	a_other.m_sampler             = vk::Sampler{};

	return *this;
}

Pipeline::~Pipeline() {
	Free();
}

void Pipeline::Free() {
	if(m_pipeline) {
		ExecuteNextFrame([pipeline = m_pipeline, pipeLineLayout = m_pipeLineLayout,
		                  descriptorSetLayout = m_descriptorSetLayout, sampler = m_sampler]() {
			auto& device = GetDevice();
			device.destroyPipeline(pipeline);
			device.destroyPipelineLayout(pipeLineLayout);
			device.destroyDescriptorSetLayout(descriptorSetLayout);
			device.destroySampler(sampler);
		});
	}
	m_pipeline            = vk::Pipeline{};
	m_pipeLineLayout      = vk::PipelineLayout{};
	m_descriptorSetLayout = vk::DescriptorSetLayout{};
	m_sampler             = vk::Sampler{};
}

void Pipeline::Frame(vk::DescriptorSet& a_set) {
	uint32_t currentVersion = TextureSets::GetCurrentVersion();
	if(currentVersion > m_lastTextureVersion) {
		std::vector<vk::ImageView> images;
		std::vector<uint16_t> imageIndices;
		TextureSets::GetImageData(images, imageIndices);
		UpdateDescriptorSet(a_set, m_sampler, {images.data(), images.size()},
		                    {imageIndices.data(), imageIndices.size()});
		m_lastTextureVersion = currentVersion;
	}
}
