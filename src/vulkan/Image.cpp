#include "Graphics/Image.h"
#include "EngineInternal.h"
#include "vulkan/vulkan.hpp"
#include <queue>

using namespace std;
using namespace CR::Graphics;

namespace {
	class VKImage : public Image {
	  public:
		VKImage(ImageType a_type, uint32_t a_width, uint32_t a_height, uint32_t a_arrayLayers, uint32_t a_mips);
		VKImage(VKImage&) = delete;
		VKImage& operator=(VKImage&) = delete;
		virtual ~VKImage();

		std::shared_ptr<ImageUpdate> UpdateImage(vector<uint8_t>&& data, uint32_t a_arrayIndex) override;

		void CreateStagingBuffer(uint32_t a_bufferSize);
		uint8_t* Map();
		void UnMap();

		vk::Image m_Image;
		vk::ImageView m_View;
		vk::DeviceMemory m_ImageMemory;

		// staging buffer will only be large enough to hold one entry in the texture array.
		// data will get queued, and loaded one at a time in a background thread.
		// Will create on first image upload(so i dont have to bother figuring out the size
		bool m_StagingBufferCreated{false};
		vk::Buffer m_StagingBuffer;
		vk::DeviceMemory m_StagingBufferMemory;

		struct ImageUpdateData {
			vector<uint8_t> m_Data;
			uint32_t m_ArrayIndex;
			std::shared_ptr<ImageUpdate> m_UpdateReady;
		};
		queue<ImageUpdateData> m_ImageUpdateQueue;
	};
}    // namespace

VKImage::VKImage(ImageType a_type, uint32_t a_width, uint32_t a_height, uint32_t a_arrayLayers, uint32_t a_mips) {
	vk::ImageCreateInfo createInfo;
	createInfo.extent.width  = a_width;
	createInfo.extent.height = a_height;
	createInfo.extent.depth  = 1;
	createInfo.arrayLayers   = a_arrayLayers;
	createInfo.mipLevels     = a_mips;
	createInfo.tiling        = vk::ImageTiling::eOptimal;
	createInfo.sharingMode   = vk::SharingMode::eExclusive;
	createInfo.usage         = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
	createInfo.initialLayout = vk::ImageLayout::eUndefined;
	createInfo.imageType     = vk::ImageType::e2D;
	createInfo.flags         = vk::ImageCreateFlagBits::e2DArrayCompatible;
	switch(a_type) {
		case ImageType::sRGBAUncompressed:
			createInfo.format = vk::Format::eA8B8G8R8SrgbPack32;
			break;
		case ImageType::sRGBACompressed:
			createInfo.format = vk::Format::eBc7SrgbBlock;
			break;
		default:
			assert(false);
			break;
	}

	m_Image = GetDevice().createImage(createInfo);

	vk::ImageViewCreateInfo viewInfo;
	viewInfo.image                         = m_Image;
	viewInfo.viewType                      = a_arrayLayers == 1 ? vk::ImageViewType::e2D : vk::ImageViewType::e2DArray;
	viewInfo.format                        = createInfo.format;
	viewInfo.subresourceRange.aspectMask   = vk::ImageAspectFlagBits::eColor;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount   = a_mips;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount     = a_arrayLayers;

	auto imageRequirements = GetDevice().getImageMemoryRequirements(m_Image);

	vk::MemoryAllocateInfo allocInfo;
	allocInfo.memoryTypeIndex = GetDeviceMemoryIndex();
	allocInfo.allocationSize  = imageRequirements.size;
	m_ImageMemory             = GetDevice().allocateMemory(allocInfo);

	GetDevice().bindImageMemory(m_Image, m_ImageMemory, 0);

	m_View = GetDevice().createImageView(viewInfo);
}

VKImage::~VKImage() {
	GetDevice().destroyImageView(m_View);
	GetDevice().destroyImage(m_Image);
	GetDevice().freeMemory(m_ImageMemory);
	GetDevice().destroyBuffer(m_StagingBuffer);
	GetDevice().freeMemory(m_StagingBufferMemory);
}

void VKImage::CreateStagingBuffer(uint32_t a_bufferSize) {
	if(m_StagingBufferCreated) { return; }

	vk::BufferCreateInfo createInfo;
	createInfo.flags       = vk::BufferCreateFlags{};
	createInfo.sharingMode = vk::SharingMode::eExclusive;
	createInfo.size        = a_bufferSize;
	createInfo.usage       = vk::BufferUsageFlagBits::eTransferSrc;

	m_StagingBuffer         = GetDevice().createBuffer(createInfo);
	auto bufferRequirements = GetDevice().getBufferMemoryRequirements(m_StagingBuffer);

	vk::MemoryAllocateInfo allocInfo;
	allocInfo.memoryTypeIndex = GetHostMemoryIndex();
	assert(bufferRequirements.alignment <= 256);
	allocInfo.allocationSize = bufferRequirements.size;
	m_StagingBufferMemory    = GetDevice().allocateMemory(allocInfo);

	GetDevice().bindBufferMemory(m_StagingBuffer, m_StagingBufferMemory, 0);

	m_StagingBufferCreated = true;
}

std::shared_ptr<CR::Graphics::Image::ImageUpdate> VKImage::UpdateImage(vector<uint8_t>&& a_data,
                                                                       uint32_t a_arrayIndex) {
	CreateStagingBuffer((uint32_t)a_data.size());

	m_ImageUpdateQueue.emplace();
	auto& update        = m_ImageUpdateQueue.back();
	update.m_ArrayIndex = a_arrayIndex;
	update.m_Data       = move(a_data);

	auto rdy = update.m_UpdateReady;
	return rdy;
}

uint8_t* VKImage::Map() {
	return (uint8_t*)GetDevice().mapMemory(m_StagingBufferMemory, 0, VK_WHOLE_SIZE);
}

void VKImage::UnMap() {
	GetDevice().unmapMemory(m_StagingBufferMemory);
}

std::unique_ptr<Image> CR::Graphics::CreateImage(ImageType a_type, uint32_t a_width, uint32_t a_height,
                                                 uint32_t a_arrayLayers, uint32_t a_mips) {
	return make_unique<VKImage>(a_type, a_width, a_height, a_arrayLayers, a_mips);
}
