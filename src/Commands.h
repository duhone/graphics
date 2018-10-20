#pragma once
#include "CommandPool.h"
#include "Graphics/Buffer.h"

namespace CR::Graphics {
	void CopyBufferToBuffer(CommandBuffer& a_cmdBuffer, Buffer& a_from, Buffer& a_to, uint32_t a_offset,
	                        uint32_t a_size);
}    // namespace CR::Graphics
