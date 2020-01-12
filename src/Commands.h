#pragma once
#include "Buffer.h"
#include "CommandPool.h"
#include "Pipeline.h"

#include "core/Span.h"

#include "glm/vec4.hpp"

#include <optional>

namespace CR::Graphics::Commands {
	void CopyBufferToBuffer(CommandBuffer& a_cmdBuffer, Buffer& a_from, Buffer& a_to, uint32_t a_offset,
	                        uint32_t a_size);
	void RenderPassBegin(CommandBuffer& a_cmdBuffer, std::optional<glm::vec4> a_clearColor);
	void RenderPassEnd(CommandBuffer& a_cmdBuffer);
	void BindPipeline(CommandBuffer& a_cmdBuffer, Pipeline& a_pipeline);
	void PushConstants(CommandBuffer& a_cmdBuffer, Pipeline& a_pipeline, CR::Core::Span<std::byte> a_data);
	void Draw(CommandBuffer& a_cmdBuffer, uint32_t a_vertexCount, uint32_t a_instanceCount);
}    // namespace CR::Graphics::Commands
