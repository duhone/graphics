#pragma once
#include "CommandPool.h"
#include "Pipeline.h"

#include "core/Span.h"

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

#include <optional>

namespace CR::Graphics::Commands {
	void RenderPassBegin(CommandBuffer& a_cmdBuffer, std::optional<glm::vec4> a_clearColor);
	void RenderPassEnd(CommandBuffer& a_cmdBuffer);
	void BindPipeline(CommandBuffer& a_cmdBuffer, Pipeline& a_pipeline);
	void BindDescriptorSet(CommandBuffer& a_cmdBuffer, const Pipeline& a_pipeline, vk::DescriptorSet& a_set,
	                       uint32_t a_offset);
	void PushConstants(CommandBuffer& a_cmdBuffer, Pipeline& a_pipeline, CR::Core::Span<std::byte> a_data);
	void Draw(CommandBuffer& a_cmdBuffer, uint32_t a_vertexCount, uint32_t a_instanceCount);

	void TransitionToDst(CommandBuffer& a_cmdBuffer, const vk::Image& a_image, vk::Format a_format,
	                     uint32_t a_layerCount);
	void CopyBufferToImg(CommandBuffer& a_cmdBuffer, const vk::Buffer& a_buffer, vk::Image& a_image,
	                     const glm::uvec2& a_extent, uint32_t layer);
	void TransitionToGraphicsQueue(CommandBuffer& a_cmdBuffer, const vk::Image& a_image, uint32_t a_layerCount);
	void TransitionFromTransferQueue(CommandBuffer& a_cmdBuffer, const vk::Image& a_image, uint32_t a_layerCount);
}    // namespace CR::Graphics::Commands
