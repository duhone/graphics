#include "Graphics/Buffer.h"
#include "EngineInternal.h"
#include "vulkan/vulkan.hpp"

using namespace std;
using namespace CR::Graphics;

namespace {
	class VKBuffer : public Buffer {
	  public:
		VKBuffer(BufferType a_type, uint a_bytes);
		VKBuffer(VKBuffer&) = delete;
		VKBuffer& operator=(VKBuffer&) = delete;
		virtual ~VKBuffer();

		vk::Buffer m_Buffer;
	};
}    // namespace

VKBuffer::VKBuffer(BufferType a_type, uint a_bytes) {
	vk::BufferCreateInfo createInfo;
	createInfo.flags       = vk::BufferCreateFlags{};
	createInfo.sharingMode = vk::SharingMode::eExclusive;
	createInfo.size        = a_bytes;
	switch(a_type) {
		case BufferType::Index:
			createInfo.usage = vk::BufferUsageFlagBits::eIndexBuffer;
			break;
		case BufferType::Vertex:
			createInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer;
			break;
		case BufferType::Uniform:
			createInfo.usage = vk::BufferUsageFlagBits::eUniformBuffer;
			break;
		default:
			assert(false);
			break;
	}

	m_Buffer = GetDevice().createBuffer(createInfo);
}

VKBuffer::~VKBuffer() {
	GetDevice().destroyBuffer(m_Buffer);
}

std::unique_ptr<Buffer> CR::Graphics::CreateBuffer(BufferType a_type, uint a_bytes) {
	return make_unique<VKBuffer>(a_type, a_bytes);
}
