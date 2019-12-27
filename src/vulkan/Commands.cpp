#include "Commands.h"
#define WIN32_LEAN_AND_MEAN
#define NOCOMM
#define NOMINMAX
#include "vulkan/vulkan.hpp"
#include <windows.h>

using namespace CR::Graphics;
using namespace std;

void CR::Graphics::CopyBufferToBuffer(CommandBuffer& a_cmdBuffer, Buffer& a_from, Buffer& a_to, uint32_t a_offset,
                                      uint32_t a_size) {
	vk::CommandBuffer* vkcmd = (vk::CommandBuffer*)a_cmdBuffer.GetHandle();
	vk::Buffer* vkfrom       = (vk::Buffer*)a_from.GetHandle();
	vk::Buffer* vkto         = (vk::Buffer*)a_to.GetHandle();

	vk::BufferCopy cpy;
	cpy.srcOffset = cpy.dstOffset = a_offset;
	cpy.size                      = a_size;
	vkcmd->copyBuffer(*vkfrom, *vkto, 1, &cpy);
}
