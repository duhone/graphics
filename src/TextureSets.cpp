#include "Graphics/TextureSet.h"

#include "TextureSets.h"
#include "vulkan/EngineInternal.h"

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

	struct TextureSetImpl {
		vector<string> m_names;
		vk::Image m_Image;
		vk::ImageView m_View;
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
}    // namespace

TextureSet ::~TextureSet() {
	if(m_id != c_unused) { g_used[m_id] = false; }
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
	for(uint32_t i = 0; i < a_textures.size(); ++i) {
		g_textureSets[set].m_names.push_back(a_textures[set].Name);
		g_lookup.emplace(a_textures[set].Name, CalcID(set, i));
	}

	TextureSet result{set};
	return result;
}

void TexturePool::Init() {
	g_used.reset();
}

void TexturePool::Shutdown() {}
