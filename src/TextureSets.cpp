#include "Graphics/TextureSet.h"

#include "TextureSets.h"
#include "vulkan/EngineInternal.h"

#include "DataCompression/LosslessCompression.h"
#include "core/Log.h"

#include "tsl/robin_map.h"

#include <bitset>
#include <unordered_map>

using namespace std;
using namespace CR;
using namespace CR::Graphics;

namespace {
	constexpr uint16_t c_maxTextureSets{8};
	constexpr uint16_t c_idSetShift{10};
	constexpr uint16_t c_maxTexturesPerSet{1024};
	static_assert((1 << c_idSetShift) == c_maxTexturesPerSet);
	static_assert(c_maxTextureSets * c_maxTexturesPerSet < numeric_limits<uint16_t>::max(),
	              "textures use a 16 bit id, some bits hold the texture set, some hold the index inside the set");

	// Must match whats in TextureProcessor, assuming dont need this every frame.
#pragma pack(4)
	struct Header {
		uint16_t Width{0};
		uint16_t Height{0};
	};
#pragma pack()

	struct TextureSetImpl {
		vector<string> m_names;
		vector<Header> m_headers;
		vector<vk::Image> m_images;
		vector<vk::ImageView> m_views;
		vk::DeviceMemory m_ImageMemory;
	};

	bitset<c_maxTextureSets> g_used;
	TextureSetImpl g_textureSets[c_maxTextureSets];
	tsl::robin_map<string, uint16_t> g_lookup;

	uint16_t CalcID(uint16_t a_set, uint16_t a_slot) {
		Core::Log::Assert(a_set < c_maxTextureSets, "invalid set");
		Core::Log::Assert(a_slot < c_maxTexturesPerSet, "invalid slot");
		return (a_set << c_idSetShift) | a_slot;
	}
	uint16_t GetSet(uint16_t a_id) { return a_id >> c_idSetShift; }
}    // namespace

TextureSet ::~TextureSet() {
	if(m_id != c_unused) {
		uint16_t set = GetSet(m_id);
		// for(auto& view : g_textureSets[set].m_views) { GetDevice().destroyImageView(view); }
		for(auto& img : g_textureSets[set].m_images) { GetDevice().destroyImage(img); }
		g_used[m_id] = false;
	}
}

TextureSet::TextureSet(TextureSet&& a_other) {
	*this = move(a_other);
}

TextureSet& TextureSet::operator=(TextureSet&& a_other) {
	m_id = a_other.m_id;

	a_other.m_id = c_unused;
	return *this;
}

TextureSet Graphics::CreateTextureSet(const Core::Span<TextureCreateInfo> a_textures) {
	if(a_textures.size() > c_maxTexturesPerSet) {
		Core::Log::Fail("Texture Sets have a maximum size of {}. {} were requested", c_maxTexturesPerSet,
		                a_textures.size());
	}
	uint16_t set = c_maxTextureSets;
	for(uint16_t i = 0; i < c_maxTextureSets; ++i) {
		if(!g_used[i]) {
			set = i;
			break;
		}
	}
	if(set == c_maxTextureSets) { Core::Log::Fail("Ran out of available texture sets"); }

	g_used[set] = true;

	g_textureSets[set].m_names.reserve(a_textures.size());
	g_textureSets[set].m_headers.reserve(a_textures.size());
	for(uint32_t slot = 0; slot < a_textures.size(); ++slot) {
		vector<byte> textureData = DataCompression::Decompress(a_textures[slot].TextureData.data(),
		                                                       (uint32_t)(a_textures[slot].TextureData.size()));
		if(textureData.size() < sizeof(Header)) { Core::Log::Fail("corrupt crtex file {}", a_textures[slot].Name); }
		Header& header = g_textureSets[set].m_headers.emplace_back();
		memcpy(&header, textureData.data(), sizeof(Header));

		g_textureSets[set].m_names.push_back(a_textures[slot].Name);
		g_lookup.emplace(a_textures[slot].Name, CalcID(set, slot));

		vk::ImageCreateInfo createInfo;
		createInfo.extent.width  = header.Width;
		createInfo.extent.height = header.Height;
		createInfo.extent.depth  = 1;
		createInfo.arrayLayers   = 1;
		createInfo.mipLevels     = 1;
		createInfo.tiling        = vk::ImageTiling::eOptimal;
		createInfo.sharingMode   = vk::SharingMode::eExclusive;
		createInfo.usage         = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
		createInfo.initialLayout = vk::ImageLayout::eUndefined;
		createInfo.imageType     = vk::ImageType::e2D;
		createInfo.flags         = vk::ImageCreateFlags{0};
		createInfo.format        = vk::Format::eBc7SrgbBlock;

		g_textureSets[set].m_images.push_back(GetDevice().createImage(createInfo));

		/*vk::ImageViewCreateInfo viewInfo;
		viewInfo.image                           = g_textureSets[set].m_images.back();
		viewInfo.viewType                        = vk::ImageViewType::e2D;
		viewInfo.format                          = createInfo.format;
		viewInfo.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
		viewInfo.subresourceRange.baseMipLevel   = 0;
		viewInfo.subresourceRange.levelCount     = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount     = 1;

		g_textureSets[set].m_views.push_back(GetDevice().createImageView(viewInfo));*/
	}

	TextureSet result{set};
	return result;
}

void TexturePool::Init() {
	g_used.reset();
}

void TexturePool::Shutdown() {}
