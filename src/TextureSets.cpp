#include "Graphics/TextureSet.h"

#include "AssetLoadingThread.h"
#include "Commands.h"
#include "Constants.h"
#include "EngineInternal.h"
#include "TextureSets.h"

#include "DataCompression/LosslessCompression.h"
#include "core/BinaryStream.h"
#include "core/Log.h"
#include "core/algorithm.h"
#include "core/literals.h"

#include <3rdParty/robinmap.h>

#include <bitset>
#include <unordered_map>

using namespace std;
using namespace CR;
using namespace CR::Graphics;
using namespace CR::Core::Literals;

namespace {
	constexpr uint16_t c_maxTextureSets{8};
	constexpr uint16_t c_idSetShift{10};
	constexpr uint16_t c_maxTexturesPerSet{1024};
	static_assert((1 << c_idSetShift) == c_maxTexturesPerSet);
	static_assert(c_maxTextureSets * c_maxTexturesPerSet < numeric_limits<uint16_t>::max(),
	              "textures use a 16 bit id, some bits hold the texture set, some hold the index inside the set");
	constexpr uint32_t c_maxStagingTextureSize = (uint32_t)16_Mb;    // enough for a 4kx4k bc7 or astc4x4
	static_assert(c_maxStagingTextureSize <= 64_Mb,
	              "Need to really think about this if there is a need to go over 64_Mb");

	// Must match whats in TextureProcessor, assuming dont need this every frame.
#pragma pack(1)
	struct Header {
		static constexpr uint32_t c_FourCC{'CRTX'};
		static constexpr uint32_t c_Version{1};
		uint32_t FourCC{c_FourCC};
		uint16_t Version{c_Version};
		uint16_t Width{0};
		uint16_t Height{0};
		uint16_t Frames{0};
	};
#pragma pack()

	struct TextureSetImpl {
		vector<string> m_names;
		vector<uint16_t> m_textureIndex;
		vector<Header> m_headers;
		vector<vk::Image> m_images;
		vector<vk::ImageView> m_views;
		vk::DeviceMemory m_imageMemory;
		vector<std::shared_ptr<std::atomic_bool>> m_loadingTask;
		vector<bool> m_ready;
	};

	uint32_t g_version{0};
	bitset<c_maxTextureSets> g_used;
	bitset<c_maxTextures> g_textureSlots;
	TextureSetImpl g_textureSets[c_maxTextureSets];
	tsl::robin_map<string, uint16_t> g_lookup;
	vk::Buffer g_stagingBuffer;
	vk::DeviceMemory g_stagingMemory;
	void* g_stagingData;

	uint16_t CalcID(uint16_t a_set, uint16_t a_slot) {
		Core::Log::Assert(a_set < c_maxTextureSets, "invalid set");
		Core::Log::Assert(a_slot < c_maxTexturesPerSet, "invalid slot");
		return (a_set << c_idSetShift) | a_slot;
	}
	uint16_t GetSet(uint16_t a_id) { return a_id >> c_idSetShift; }
	uint16_t GetSlot(uint16_t a_id) { return a_id & (c_maxTexturesPerSet - 1); }
}    // namespace

TextureSet ::~TextureSet() {
	if(m_id != c_unused) {
		uint16_t set = GetSet(m_id);
		while(!Core::all_of(g_textureSets[set].m_ready, [](const auto& a_rdy) { return a_rdy; })) {
			this_thread::sleep_for(64ms);

			for(uint32_t slot = 0; slot < g_textureSets[set].m_ready.size(); ++slot) {
				if(!g_textureSets[set].m_ready[slot]) {
					if(g_textureSets[set].m_loadingTask[slot]->load(memory_order_acquire)) {
						g_textureSets[set].m_loadingTask[slot].reset();
						g_textureSets[set].m_ready[slot] = true;
					}
				}
			}
		}

		auto& device = GetDevice();
		for(auto& view : g_textureSets[set].m_views) { device.destroyImageView(view); }
		for(auto& img : g_textureSets[set].m_images) { device.destroyImage(img); }
		device.freeMemory(g_textureSets[set].m_imageMemory);

		for(const auto& name : g_textureSets[set].m_names) { g_lookup.erase(name); }

		g_textureSets[set].m_names.clear();
		g_textureSets[set].m_headers.clear();
		g_textureSets[set].m_images.clear();
		g_textureSets[set].m_views.clear();
		g_textureSets[set].m_loadingTask.clear();
		g_textureSets[set].m_ready.clear();
		for(const auto& slot : g_textureSets[set].m_textureIndex) { g_textureSlots[slot] = false; }
		g_textureSets[set].m_textureIndex.clear();
		g_used[m_id] = false;

		++g_version;
	}
}

TextureSet::TextureSet(TextureSet&& a_other) noexcept {
	*this = move(a_other);
}

TextureSet& TextureSet::operator=(TextureSet&& a_other) noexcept {
	m_id = a_other.m_id;

	a_other.m_id = c_unused;
	return *this;
}

void Graphics::TextureSets::CheckLoadingTasks(CommandBuffer& a_cmdBuffer) {
	for(uint32_t set = 0; set < c_maxTextureSets; ++set) {
		if(g_used[set]) {
			for(uint32_t slot = 0; slot < g_textureSets[set].m_ready.size(); ++slot) {
				if(!g_textureSets[set].m_ready[slot]) {
					if(g_textureSets[set].m_loadingTask[slot]->load(memory_order_acquire)) {
						g_textureSets[set].m_loadingTask[slot].reset();
						Commands::TransitionFromTransferQueue(a_cmdBuffer, g_textureSets[set].m_images[slot],
						                                      g_textureSets[set].m_headers[slot].Frames);
						g_textureSets[set].m_ready[slot] = true;
					}
				}
			}
		}
	}
}

TextureSet::TextureSet(const Core::Span<TextureCreateInfo> a_textures) {
	Core::Log::Require(a_textures.size() <= c_maxTexturesPerSet,
	                   "Texture Sets have a maximum size of {}. {} was requested", c_maxTexturesPerSet,
	                   a_textures.size());

	uint16_t set = c_maxTextureSets;
	for(uint16_t i = 0; i < c_maxTextureSets; ++i) {
		if(!g_used[i]) {
			set = i;
			break;
		}
	}
	Core::Log::Require(set != c_maxTextureSets, "Ran out of available texture sets");

	g_used[set] = true;

	vector<uint32_t> memOffsets;
	memOffsets.reserve(a_textures.size());
	uint32_t memOffset{0};

	auto& device = GetDevice();

	g_textureSets[set].m_names.reserve(a_textures.size());
	g_textureSets[set].m_textureIndex.reserve(a_textures.size());
	g_textureSets[set].m_headers.reserve(a_textures.size());
	g_textureSets[set].m_images.reserve(a_textures.size());
	g_textureSets[set].m_views.reserve(a_textures.size());
	g_textureSets[set].m_loadingTask.reserve(a_textures.size());
	g_textureSets[set].m_ready.reserve(a_textures.size());
	vector<vector<byte>> textureDataList;
	for(uint32_t slot = 0; slot < a_textures.size(); ++slot) {
		uint16_t descSlot = c_maxTextures;
		for(uint16_t i = 0; i < c_maxTextures; ++i) {
			if(!g_textureSlots[i]) {
				descSlot          = i;
				g_textureSlots[i] = true;
				break;
			}
		}
		Core::Log::Require(descSlot != c_maxTextures, "Ran out of available texture descriptor slots");

		vector<byte> textureData;
		textureData.insert(begin(textureData), a_textures[slot].TextureData.data(),
		                   a_textures[slot].TextureData.data() + a_textures[slot].TextureData.size());
		Core::Log::Require(textureData.size() >= sizeof(Header), "corrupt crtex file {}", a_textures[slot].Name);

		Core::Log::Require(textureData.size() <= c_maxStagingTextureSize, "texture is too large {}",
		                   a_textures[slot].Name);

		Header& header = g_textureSets[set].m_headers.emplace_back();
		memcpy(&header, textureData.data(), sizeof(Header));
		Core::Log::Require(header.FourCC == Header::c_FourCC, "texture is not a crtexd fourcc is wrong");
		Core::Log::Require(header.Version == Header::c_Version,
		                   "texture is not the correct version, rebuild the texture");

		g_textureSets[set].m_names.push_back(a_textures[slot].Name);
		g_lookup.emplace(a_textures[slot].Name, CalcID(set, (uint16_t)slot));

		vk::ImageCreateInfo createInfo;
		createInfo.extent.width  = header.Width;
		createInfo.extent.height = header.Height;
		createInfo.extent.depth  = 1;
		createInfo.arrayLayers   = header.Frames;
		createInfo.mipLevels     = 1;
		createInfo.tiling        = vk::ImageTiling::eOptimal;
		createInfo.sharingMode   = vk::SharingMode::eExclusive;
		createInfo.usage         = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
		createInfo.initialLayout = vk::ImageLayout::eUndefined;
		createInfo.imageType     = vk::ImageType::e2D;
		createInfo.flags         = vk::ImageCreateFlags{0};
		createInfo.format        = vk::Format::eBc7SrgbBlock;

		g_textureSets[set].m_images.push_back(device.createImage(createInfo));

		auto imageRequirements = device.getImageMemoryRequirements(g_textureSets[set].m_images.back());
		memOffset = (uint32_t)((memOffset + (imageRequirements.alignment - 1)) & ~(imageRequirements.alignment - 1));
		memOffsets.push_back(memOffset);
		memOffset += (uint32_t)imageRequirements.size;

		textureDataList.push_back(move(textureData));

		g_textureSets[set].m_textureIndex.push_back(descSlot);
	}

	vk::MemoryAllocateInfo allocInfo;
	allocInfo.memoryTypeIndex        = GetDeviceMemoryIndex();
	allocInfo.allocationSize         = memOffset;
	g_textureSets[set].m_imageMemory = device.allocateMemory(allocInfo);

	for(uint32_t slot = 0; slot < a_textures.size(); ++slot) {
		device.bindImageMemory(g_textureSets[set].m_images[slot], g_textureSets[set].m_imageMemory, memOffsets[slot]);
		vk::ImageViewCreateInfo viewInfo;
		viewInfo.image                           = g_textureSets[set].m_images[slot];
		viewInfo.viewType                        = vk::ImageViewType::e2DArray;
		viewInfo.format                          = vk::Format::eBc7SrgbBlock;
		viewInfo.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
		viewInfo.subresourceRange.baseMipLevel   = 0;
		viewInfo.subresourceRange.levelCount     = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount     = g_textureSets[set].m_headers[slot].Frames;

		g_textureSets[set].m_views.push_back(device.createImageView(viewInfo));
	}

	for(uint32_t slot = 0; slot < a_textures.size(); ++slot) {
		g_textureSets[set].m_ready.push_back(false);
		g_textureSets[set].m_loadingTask.push_back(AssetLoadingThread::LoadAsset(
		    [textureData = move(textureDataList[slot]), set, slot](auto getCmdBuffer, auto submit) {
			    Core::BinaryReader reader;
			    reader.Data = textureData.data();
			    reader.Size = (uint32_t)textureData.size();

			    Header header;
			    Core::Read(reader, header);

			    {
				    CommandBuffer& cmdBuffer = getCmdBuffer();
				    Commands::TransitionToDst(cmdBuffer, g_textureSets[set].m_images[slot], header.Frames);
				    submit();
			    }
			    for(uint32_t i = 0; i < header.Frames; ++i) {
				    CommandBuffer& cmdBuffer = getCmdBuffer();

				    std::vector<std::byte> compressedData;
				    Core::Read(reader, compressedData);

				    Core::storage_buffer<byte> uncompressedData = DataCompression::Decompress(
				        CR::Core::Span<const byte>(compressedData.data(), compressedData.size()));

				    memcpy(g_stagingData, uncompressedData.data(), uncompressedData.size());

				    Commands::CopyBufferToImg(cmdBuffer, g_stagingBuffer, g_textureSets[set].m_images[slot],
				                              {header.Width, header.Height}, i);
				    submit();
			    }
			    {
				    CommandBuffer& cmdBuffer = getCmdBuffer();
				    Commands::TransitionToGraphicsQueue(cmdBuffer, g_textureSets[set].m_images[slot], header.Frames);
				    submit();
			    }
		    }));
	}
	textureDataList.clear();

	++g_version;

	m_id = set;
}

void TextureSets::Init() {
	g_used.reset();

	vk::BufferCreateInfo stagInfo;
	stagInfo.flags       = vk::BufferCreateFlags{};
	stagInfo.sharingMode = vk::SharingMode::eExclusive;
	stagInfo.size        = c_maxStagingTextureSize;
	stagInfo.usage       = vk::BufferUsageFlagBits::eTransferSrc;

	auto& device            = GetDevice();
	g_stagingBuffer         = device.createBuffer(stagInfo);
	auto bufferRequirements = device.getBufferMemoryRequirements(g_stagingBuffer);

	vk::MemoryAllocateInfo allocInfo;
	allocInfo.memoryTypeIndex = GetHostMemoryIndex();
	allocInfo.allocationSize  = bufferRequirements.size;
	g_stagingMemory           = device.allocateMemory(allocInfo);
	device.bindBufferMemory(g_stagingBuffer, g_stagingMemory, 0);

	g_stagingData = device.mapMemory(g_stagingMemory, 0, VK_WHOLE_SIZE);
}

void TextureSets::Shutdown() {
	auto& device = GetDevice();
	device.unmapMemory(g_stagingMemory);
	device.freeMemory(g_stagingMemory);
	device.destroyBuffer(g_stagingBuffer);
}

uint32_t TextureSets::GetCurrentVersion() {
	return g_version;
}

void TextureSets::GetImageData(std::vector<vk::ImageView>& a_images, std::vector<uint16_t>& a_imageIndices) {
	for(uint32_t set = 0; set < c_maxTextureSets; ++set) {
		if(g_used[set]) {
			a_images.insert(end(a_images), g_textureSets[set].m_views.begin(), g_textureSets[set].m_views.end());
			a_imageIndices.insert(end(a_imageIndices), g_textureSets[set].m_textureIndex.begin(),
			                      g_textureSets[set].m_textureIndex.end());
		}
	}
}

uint16_t TextureSets::GetTextureIndex(const char* a_textureName) {
	auto texIter = g_lookup.find(a_textureName);
	Core::Log::Assert(texIter != g_lookup.end(), "Requested a texture {} that hasn't been loaded", a_textureName);

	return texIter->second;
}

uint16_t TextureSets::GetMaxFrames(uint16_t a_textureIndex) {
	auto set  = GetSet(a_textureIndex);
	auto slot = GetSlot(a_textureIndex);

	return g_textureSets[set].m_headers[slot].Frames;
}
