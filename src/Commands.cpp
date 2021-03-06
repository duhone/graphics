﻿#include "Commands.h"

#include "EngineInternal.h"

#include "VulkanWindows.h"

using namespace CR::Graphics;
using namespace std;

void Commands::RenderPassBegin(CommandBuffer& a_cmdBuffer, std::optional<glm::vec4> a_clearColor) {
	vk::RenderPassBeginInfo renderPassInfo;
	renderPassInfo.renderPass               = GetRenderPass();
	renderPassInfo.renderArea.extent.width  = GetWindowSize().x;
	renderPassInfo.renderArea.extent.height = GetWindowSize().y;
	renderPassInfo.framebuffer              = GetFrameBuffer();
	vk::ClearValue clearValue;
	if(a_clearColor.has_value()) {
		clearValue.color.setFloat32(
		    {a_clearColor.value().r, a_clearColor.value().g, a_clearColor.value().b, a_clearColor.value().a});
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues    = &clearValue;

	} else {
		renderPassInfo.clearValueCount = 0;
	}

	vk::CommandBuffer& vkcmd = a_cmdBuffer.GetHandle();
	vkcmd.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
}

void Commands::RenderPassEnd(CommandBuffer& a_cmdBuffer) {
	vk::CommandBuffer& vkcmd = a_cmdBuffer.GetHandle();
	vkcmd.endRenderPass();
}

void Commands::BindPipeline(CommandBuffer& a_cmdBuffer, Pipeline& a_pipeline) {
	vk::CommandBuffer& vkcmd = a_cmdBuffer.GetHandle();
	vkcmd.bindPipeline(vk::PipelineBindPoint::eGraphics, a_pipeline.GetHandle());
}

void Commands::BindVertexBuffer(CommandBuffer& a_cmdBuffer, const vk::Buffer& a_buffer) {
	vk::CommandBuffer& vkcmd = a_cmdBuffer.GetHandle();
	vkcmd.bindVertexBuffers(0, {a_buffer}, {0});
}

void Commands::PushConstants(CommandBuffer& a_cmdBuffer, Pipeline& a_pipeline, CR::Core::Span<std::byte> a_data) {
	vk::CommandBuffer& vkcmd = a_cmdBuffer.GetHandle();
	vkcmd.pushConstants(a_pipeline.GetLayout(), vk::ShaderStageFlagBits::eVertex, 0, (uint32_t)a_data.size(),
	                    a_data.data());
}

void Commands::Draw(CommandBuffer& a_cmdBuffer, uint32_t a_vertexCount, uint32_t a_instanceCount) {
	vk::CommandBuffer& vkcmd = a_cmdBuffer.GetHandle();
	vkcmd.draw(a_vertexCount, a_instanceCount, 0, 0);
}

void Commands::BindDescriptorSet(CommandBuffer& a_cmdBuffer, const Pipeline& a_pipeline, vk::DescriptorSet& a_set) {
	vk::CommandBuffer& vkcmd = a_cmdBuffer.GetHandle();
	vkcmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, a_pipeline.GetLayout(), 0, 1, &a_set, 0, nullptr);
}

void Commands::TransitionToDst(CommandBuffer& a_cmdBuffer, const vk::Image& a_image, uint32_t a_layerCount) {
	vk::CommandBuffer& vkcmd = a_cmdBuffer.GetHandle();

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

	vkcmd.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer,
	                      vk::DependencyFlags{}, nullptr, nullptr, barrier);
}

void Commands::CopyBufferToImg(CommandBuffer& a_cmdBuffer, const vk::Buffer& a_buffer, vk::Image& a_image,
                               const glm::uvec2& a_extent, uint32_t layer) {
	vk::CommandBuffer& vkcmd = a_cmdBuffer.GetHandle();

	vk::ImageLayout layout{vk::ImageLayout::eTransferDstOptimal};

	vk::BufferImageCopy cpy;
	cpy.bufferOffset                    = 0;
	cpy.bufferRowLength                 = 0;
	cpy.bufferImageHeight               = 0;
	cpy.imageSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
	cpy.imageSubresource.baseArrayLayer = layer;
	cpy.imageSubresource.layerCount     = 1;
	cpy.imageSubresource.mipLevel       = 0;
	cpy.imageOffset                     = vk::Offset3D{0, 0, 0};
	cpy.imageExtent                     = vk::Extent3D{a_extent.x, a_extent.y, 1};

	vkcmd.copyBufferToImage(a_buffer, a_image, layout, cpy);
}

void Commands::TransitionToGraphicsQueue(CommandBuffer& a_cmdBuffer, const vk::Image& a_image, uint32_t a_layerCount) {
	vk::CommandBuffer& vkcmd = a_cmdBuffer.GetHandle();

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

	vkcmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eBottomOfPipe,
	                      vk::DependencyFlags{}, nullptr, nullptr, barrier);
}

void Commands::TransitionFromTransferQueue(CommandBuffer& a_cmdBuffer, const vk::Image& a_image,
                                           uint32_t a_layerCount) {
	vk::CommandBuffer& vkcmd = a_cmdBuffer.GetHandle();

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

	vkcmd.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eFragmentShader,
	                      vk::DependencyFlags{}, nullptr, nullptr, barrier);
}

void Commands::CopyBufferToBuffer(CommandBuffer& a_cmdBuffer, const vk::Buffer& a_bufferSrc, vk::Buffer& a_bufferDst,
                                  uint32_t a_size) {
	vk::CommandBuffer& vkcmd = a_cmdBuffer.GetHandle();

	vk::BufferCopy cpy;
	cpy.srcOffset = 0;
	cpy.dstOffset = 0;
	cpy.size      = a_size;

	vkcmd.copyBuffer(a_bufferSrc, a_bufferDst, cpy);
}

void Commands::SetEvent(CommandBuffer& a_cmdBuffer, const vk::Event& a_event) {
	vk::CommandBuffer& vkcmd = a_cmdBuffer.GetHandle();
	vkcmd.setEvent(a_event, vk::PipelineStageFlagBits::eTransfer);
}

void Commands::WaitEvent(CommandBuffer& a_cmdBuffer, const vk::Event& a_event) {
	vk::CommandBuffer& vkcmd = a_cmdBuffer.GetHandle();

	vk::MemoryBarrier memBarrier;
	memBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
	memBarrier.dstAccessMask = vk::AccessFlagBits::eVertexAttributeRead;

	vkcmd.waitEvents(a_event, vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eVertexInput, memBarrier,
	                 nullptr, nullptr);
}
