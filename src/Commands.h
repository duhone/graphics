#pragma once
#include "Buffer.h"
#include "CommandPool.h"

namespace CR::Graphics {
	void CopyBufferToBuffer(CommandBuffer& a_cmdBuffer, Buffer& a_from, Buffer& a_to, uint32_t a_offset,
	                        uint32_t a_size);
}    // namespace CR::Graphics
