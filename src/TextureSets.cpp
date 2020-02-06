#include "Graphics/TextureSet.h"

#include "AssetLoadingThread.h"
#include "Commands.h"
#include "TextureSets.h"
#include "vulkan/EngineInternal.h"

#include "DataCompression/LosslessCompression.h"
#include "core/Log.h"
#include "core/algorithm.h"

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
		vk::DeviceMemory m_imageMemory;
		vector<std::shared_ptr<std::atomic_bool>> m_loadingTask;
		vector<bool> m_ready;
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
	uint16_t GetSlot(uint16_t a_id) { return a_id & (c_maxTexturesPerSet - 1); }
}    // namespace

TextureSet ::~TextureSet() {
	if(m_id != c_unused) {
		uint16_t set = GetSet(m_id);
		while(!Core::all_of(g_textureSets[set].m_ready, [](const auto& a_rdy) { return a_rdy; })) {
			TextureSets::CheckLoadingTasks();
			this_thread::sleep_for(64ms);
		}
		GetDevice([&](auto& a_device) {
			for(auto& view : g_textureSets[set].m_views) { a_device.destroyImageView(view); }
			for(auto& img : g_textureSets[set].m_images) { a_device.destroyImage(img); }
			a_device.freeMemory(g_textureSets[set].m_imageMemory);
		});
		g_textureSets[set].m_names.clear();
		g_textureSets[set].m_headers.clear();
		g_textureSets[set].m_images.clear();
		g_textureSets[set].m_views.clear();
		g_textureSets[set].m_loadingTask.clear();
		g_textureSets[set].m_ready.clear();
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

void Graphics::TextureSets::CheckLoadingTasks() {
	for(uint32_t set = 0; set < c_maxTextureSets; ++set) {
		if(g_used[set]) {
			for(uint32_t slot = 0; slot < g_textureSets[set].m_ready.size(); ++slot) {
				if(!g_textureSets[set].m_ready[slot]) {
					if(g_textureSets[set].m_loadingTask[slot]->load(memory_order_acquire)) {
						g_textureSets[set].m_ready[slot] = true;
						g_textureSets[set].m_loadingTask[slot].reset();
					}
				}
			}
		}
	}
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

	vector<uint32_t> memOffsets;
	memOffsets.reserve(a_textures.size());
	uint32_t memOffset{0};

	g_textureSets[set].m_names.reserve(a_textures.size());
	g_textureSets[set].m_headers.reserve(a_textures.size());
	g_textureSets[set].m_images.reserve(a_textures.size());
	g_textureSets[set].m_views.reserve(a_textures.size());
	g_textureSets[set].m_loadingTask.reserve(a_textures.size());
	g_textureSets[set].m_ready.reserve(a_textures.size());
	vector<vector<byte>> textureDataList;
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

		GetDevice([&](auto& a_device) {
			g_textureSets[set].m_images.push_back(a_device.createImage(createInfo));

			auto imageRequirements = a_device.getImageMemoryRequirements(g_textureSets[set].m_images.back());
			memOffset =
			    (uint32_t)((memOffset + (imageRequirements.alignment - 1)) & ~(imageRequirements.alignment - 1));
			memOffsets.push_back(memOffset);
			memOffset += (uint32_t)imageRequirements.size;
		});
		textureDataList.push_back(move(textureData));
	}

	vk::MemoryAllocateInfo allocInfo;
	GetDevice([&](auto& a_device) {
		allocInfo.memoryTypeIndex        = GetDeviceMemoryIndex();
		allocInfo.allocationSize         = memOffset;
		g_textureSets[set].m_imageMemory = a_device.allocateMemory(allocInfo);
	});

	for(uint32_t slot = 0; slot < a_textures.size(); ++slot) {
		GetDevice([&](auto& a_device) {
			a_device.bindImageMemory(g_textureSets[set].m_images[slot], g_textureSets[set].m_imageMemory,
			                         memOffsets[slot]);
			vk::ImageViewCreateInfo viewInfo;
			viewInfo.image                           = g_textureSets[set].m_images[slot];
			viewInfo.viewType                        = vk::ImageViewType::e2D;
			viewInfo.format                          = vk::Format::eBc7SrgbBlock;
			viewInfo.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
			viewInfo.subresourceRange.baseMipLevel   = 0;
			viewInfo.subresourceRange.levelCount     = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount     = 1;

			g_textureSets[set].m_views.push_back(a_device.createImageView(viewInfo));
		});
	}
	for(uint32_t slot = 0; slot < a_textures.size(); ++slot) {
		g_textureSets[set].m_ready.push_back(false);
		g_textureSets[set].m_loadingTask.push_back(AssetLoadingThread::LoadAsset(
		    [textureData = move(textureDataList[slot]), set, slot](CommandBuffer& a_buffer) {
			    vk::BufferCreateInfo stagInfo;
			    stagInfo.flags       = vk::BufferCreateFlags{};
			    stagInfo.sharingMode = vk::SharingMode::eExclusive;
			    stagInfo.size        = textureData.size() - sizeof(Header);
			    stagInfo.usage       = vk::BufferUsageFlagBits::eTransferSrc;

			    vk::Buffer stagingBuffer;
			    vk::DeviceMemory memory;
			    void* memoryPtr;
			    GetDevice([&stagInfo, &stagingBuffer, &memory, &memoryPtr](vk::Device& a_device) {
				    stagingBuffer           = a_device.createBuffer(stagInfo);
				    auto bufferRequirements = a_device.getBufferMemoryRequirements(stagingBuffer);

				    vk::MemoryAllocateInfo allocInfo;
				    allocInfo.memoryTypeIndex = GetHostMemoryIndex();
				    allocInfo.allocationSize  = bufferRequirements.size;
				    memory                    = a_device.allocateMemory(allocInfo);
				    a_device.bindBufferMemory(stagingBuffer, memory, 0);

				    memoryPtr = a_device.mapMemory(memory, 0, VK_WHOLE_SIZE);
			    });

			    memcpy(memoryPtr, textureData.data() + sizeof(Header), textureData.size() - sizeof(Header));

			    GetDevice([&memory](vk::Device& a_device) { a_device.unmapMemory(memory); });

			    Header header = g_textureSets[set].m_headers[slot];
			    Commands::TransitionToDst(a_buffer, g_textureSets[set].m_images[slot], vk::Format::eBc7SrgbBlock);
			    Commands::CopyBufferToImg(a_buffer, stagingBuffer, g_textureSets[set].m_images[slot],
			                              {header.Width, header.Height});

			    return [stagingBuffer, memory]() {
				    GetDevice([&stagingBuffer, &memory](vk::Device& a_device) {
					    a_device.freeMemory(memory);
					    a_device.destroyBuffer(stagingBuffer);
				    });
			    };
		    }));
	}
	textureDataList.clear();

	TextureSet result{set};
	return result;
}

void TextureSets::Init() {
	g_used.reset();
}

void TextureSets::Shutdown() {}
