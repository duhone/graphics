#include "Graphics/Image.h"
#include "EngineInternal.h"
#include "vulkan/vulkan.hpp"

using namespace std;
using namespace CR::Graphics;

namespace {
	class VKImage : public Image {
	  public:
		VKImage(ImageType a_type, uint a_width, uint a_height, uint a_arrayLayers, uint a_mips);
		VKImage(VKImage&) = delete;
		VKImage& operator=(VKImage&) = delete;
		virtual ~VKImage();

		vk::Image m_Image;
	};
}    // namespace

VKImage::VKImage(ImageType a_type, uint a_width, uint a_height, uint a_arrayLayers, uint a_mips) {
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
		case ImageType::AsRGB:
			createInfo.format = vk::Format::eA8B8G8R8SrgbPack32;
			break;
		case ImageType::BC1:
			createInfo.format = vk::Format::eBc1RgbSrgbBlock;
			break;
		case ImageType::BC3:
			createInfo.format = vk::Format::eBc3SrgbBlock;
			break;
		case ImageType::BC4:
			createInfo.format = vk::Format::eBc4UnormBlock;
			break;
		case ImageType::BC5:
			createInfo.format = vk::Format::eBc5UnormBlock;
			break;
		default:
			assert(false);
			break;
	}

	m_Image = GetDevice().createImage(createInfo);
}

VKImage::~VKImage() {
	GetDevice().destroyImage(m_Image);
}

std::unique_ptr<Image> CR::Graphics::CreateImage(ImageType a_type, uint a_width, uint a_height, uint a_arrayLayers,
                                                 uint a_mips) {
	return make_unique<VKImage>(a_type, a_width, a_height, a_arrayLayers, a_mips);
}
