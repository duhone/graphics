#include "Graphics/TextureSet.h"

#include "TextureSets.h"

#include <bitset>

using namespace std;
using namespace CR;
using namespace CR::Graphics;

namespace {
	constexpr uint16_t c_maxTextureSets{8};

	struct TextureSetImpl {
		vector<string> m_names;
	};

	bitset<c_maxTextureSets> g_used;
	TextureSetImpl g_textureSets[c_maxTextureSets];
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
	uint16_t slot = c_maxTextureSets;
	for(uint16_t i = 0; i < c_maxTextureSets; ++i) {
		if(!g_used[i]) {
			slot = i;
			break;
		}
	}
	if(slot == c_maxTextureSets) { Core::Log::Fail("Ran out of available texture sets"); }

	g_used[slot] = true;

	for(uint32_t i = 0; i < a_textures.size(); ++i) { g_textureSets[slot].m_names.push_back(a_textures[slot].Name); }

	TextureSet result{slot};
	return result;
}

void TexturePool::Init() {
	g_used.reset();
}

void TexturePool::Shutdown() {}
