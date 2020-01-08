#include "Commands.h"

#include "EngineInternal.h"

#define WIN32_LEAN_AND_MEAN
#define NOCOMM
#define NOMINMAX
#include "vulkan/vulkan.hpp"
#include <windows.h>

using namespace CR::Graphics;
using namespace std;

void Commands::CopyBufferToBuffer(CommandBuffer& a_cmdBuffer, Buffer& a_from, Buffer& a_to, uint32_t a_offset,
                                  uint32_t a_size) {
	vk::CommandBuffer* vkcmd = (vk::CommandBuffer*)a_cmdBuffer.GetHandle();
	vk::Buffer* vkfrom       = (vk::Buffer*)a_from.GetHandle();
	vk::Buffer* vkto         = (vk::Buffer*)a_to.GetHandle();

	vk::BufferCopy cpy;
	cpy.srcOffset = cpy.dstOffset = a_offset;
	cpy.size                      = a_size;
	vkcmd->copyBuffer(*vkfrom, *vkto, 1, &cpy);
}

void Commands::RenderPassBegin(CommandBuffer& a_cmdBuffer, std::optional<glm::vec4> a_clearColor) {
	vk::RenderPassBeginInfo renderPassInfo;
	renderPassInfo.renderPass               = GetRenderPass();
	renderPassInfo.renderArea.extent.width  = GetWindowSize().x;
	renderPassInfo.renderArea.extent.height = GetWindowSize().y;
	renderPassInfo.framebuffer              = GetFrameBuffer();
	if(a_clearColor.has_value()) {
		vk::ClearValue clearValue;
		clearValue.color.setFloat32(
		    {a_clearColor.value().r, a_clearColor.value().g, a_clearColor.value().b, a_clearColor.value().a});
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues    = &clearValue;

	} else {
		renderPassInfo.clearValueCount = 0;
	}

	vk::CommandBuffer* vkcmd = (vk::CommandBuffer*)a_cmdBuffer.GetHandle();
	vkcmd->beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
}

void Commands::RenderPassEnd(CommandBuffer& a_cmdBuffer) {
	vk::CommandBuffer* vkcmd = (vk::CommandBuffer*)a_cmdBuffer.GetHandle();
	vkcmd->endRenderPass();
}

void Commands::BindPipeline(CommandBuffer& a_cmdBuffer, Pipeline& a_pipeline) {
	vk::CommandBuffer* vkcmd = (vk::CommandBuffer*)a_cmdBuffer.GetHandle();
	vkcmd->bindPipeline(vk::PipelineBindPoint::eGraphics, *(const vk::Pipeline*)a_pipeline.GetHandle());
}

void Commands::Draw(CommandBuffer& a_cmdBuffer, uint32_t a_vertexCount, uint32_t a_instanceCount) {
	vk::CommandBuffer* vkcmd = (vk::CommandBuffer*)a_cmdBuffer.GetHandle();
	vkcmd->draw(a_vertexCount, a_instanceCount, 0, 0);
}
