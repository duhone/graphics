#include "Commands.h"

#include "EngineInternal.h"

#define WIN32_LEAN_AND_MEAN
#define NOCOMM
#define NOMINMAX
#include "vulkan/vulkan.hpp"
#include <windows.h>

using namespace CR::Graphics;
using namespace std;

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
	vkcmd->bindPipeline(vk::PipelineBindPoint::eGraphics, a_pipeline.GetHandle());
}

void Commands::PushConstants(CommandBuffer& a_cmdBuffer, Pipeline& a_pipeline, CR::Core::Span<std::byte> a_data) {
	vk::CommandBuffer* vkcmd = (vk::CommandBuffer*)a_cmdBuffer.GetHandle();
	vkcmd->pushConstants(a_pipeline.GetLayout(), vk::ShaderStageFlagBits::eVertex, 0, (uint32_t)a_data.size(),
	                     a_data.data());
}

void Commands::Draw(CommandBuffer& a_cmdBuffer, uint32_t a_vertexCount, uint32_t a_instanceCount) {
	vk::CommandBuffer* vkcmd = (vk::CommandBuffer*)a_cmdBuffer.GetHandle();
	vkcmd->draw(a_vertexCount, a_instanceCount, 0, 0);
}

void Commands::BindDescriptorSet(CommandBuffer& a_cmdBuffer, const Pipeline& a_pipeline, vk::DescriptorSet& a_set,
                                 uint32_t a_offset) {
	vk::CommandBuffer* vkcmd = (vk::CommandBuffer*)a_cmdBuffer.GetHandle();
	vkcmd->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, a_pipeline.GetLayout(), 0, 1, &a_set, 1, &a_offset);
}

void Commands::TransitionToDst(CommandBuffer& a_cmdBuffer, const vk::Image& a_image, vk::Format a_format,
                               uint32_t a_layerCount) {
	vk::CommandBuffer* vkcmd = (vk::CommandBuffer*)a_cmdBuffer.GetHandle();

	vk::ImageMemoryBarrier barrier;
	barrier.image                           = a_image;
	barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
	barrier.srcAccessMask                   = vk::AccessFlags{};
	barrier.dstAccessMask                   = vk::AccessFlagBits::eTransferWrite;
	barrier.oldLayout                       = vk::ImageLayout::eUndefined;
	barrier.newLayout                       = vk::ImageLayout::eTransferDstOptimal;
	barrier.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount     = a_layerCount;
	barrier.subresourceRange.baseMipLevel   = 0;
	barrier.subresourceRange.levelCount     = 1;

	vkcmd->pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer,
	                       vk::DependencyFlags{}, nullptr, nullptr, barrier);
}

void Commands::CopyBufferToImg(CommandBuffer& a_cmdBuffer, const vk::Buffer& a_buffer, vk::Image& a_image,
                               const glm::uvec2& a_extent, uint32_t layer) {
	vk::CommandBuffer* vkcmd = (vk::CommandBuffer*)a_cmdBuffer.GetHandle();

	vk::ImageLayout layout{vk::ImageLayout::eTransferDstOptimal};

	vk::BufferImageCopy cpy;
	cpy.bufferOffset                    = 0;
	cpy.bufferRowLength                 = 0;
	cpy.bufferImageHeight               = 0;
	cpy.imageSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
	cpy.imageSubresource.baseArrayLayer = layer;
	cpy.imageSubresource.layerCount     = 1;
	cpy.imageSubresource.mipLevel       = 0;
	cpy.imageOffset                     = {0, 0, 0};
	cpy.imageExtent                     = {a_extent.x, a_extent.y, 1};

	vkcmd->copyBufferToImage(a_buffer, a_image, layout, cpy);
}

void Commands::TransitionToGraphicsQueue(CommandBuffer& a_cmdBuffer, const vk::Image& a_image, uint32_t a_layerCount) {
	vk::CommandBuffer* vkcmd = (vk::CommandBuffer*)a_cmdBuffer.GetHandle();

	vk::ImageMemoryBarrier barrier;
	barrier.image                           = a_image;
	barrier.srcQueueFamilyIndex             = GetTransferQueueIndex();
	barrier.dstQueueFamilyIndex             = GetGraphicsQueueIndex();
	barrier.srcAccessMask                   = vk::AccessFlagBits::eTransferWrite;
	barrier.dstAccessMask                   = vk::AccessFlags{};
	barrier.oldLayout                       = vk::ImageLayout::eTransferDstOptimal;
	barrier.newLayout                       = vk::ImageLayout::eShaderReadOnlyOptimal;
	barrier.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount     = a_layerCount;
	barrier.subresourceRange.baseMipLevel   = 0;
	barrier.subresourceRange.levelCount     = 1;

	vkcmd->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eBottomOfPipe,
	                       vk::DependencyFlags{}, nullptr, nullptr, barrier);
}

void Commands::TransitionFromTransferQueue(CommandBuffer& a_cmdBuffer, const vk::Image& a_image,
                                           uint32_t a_layerCount) {
	vk::CommandBuffer* vkcmd = (vk::CommandBuffer*)a_cmdBuffer.GetHandle();

	vk::ImageMemoryBarrier barrier;
	barrier.image                           = a_image;
	barrier.srcQueueFamilyIndex             = GetTransferQueueIndex();
	barrier.dstQueueFamilyIndex             = GetGraphicsQueueIndex();
	barrier.srcAccessMask                   = vk::AccessFlags{};
	barrier.dstAccessMask                   = vk::AccessFlagBits::eShaderRead;
	barrier.oldLayout                       = vk::ImageLayout::eTransferDstOptimal;
	barrier.newLayout                       = vk::ImageLayout::eShaderReadOnlyOptimal;
	barrier.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount     = a_layerCount;
	barrier.subresourceRange.baseMipLevel   = 0;
	barrier.subresourceRange.levelCount     = 1;

	vkcmd->pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eFragmentShader,
	                       vk::DependencyFlags{}, nullptr, nullptr, barrier);
}
