#include "Graphics/Texture.h"

#include "TexturePool.h"

#include <bitset>

using namespace std;
using namespace CR;
using namespace CR::Graphics;

namespace {
	constexpr uint16_t c_maxTextures{256};

	bitset<c_maxTextures> g_used;
	string g_names[c_maxTextures];
}    // namespace

Texture ::~Texture() {
	if(m_id != c_unused) { g_used[m_id] = false; }
}

Texture::Texture(Texture&& a_other) {
	*this = move(a_other);
}

Texture& Texture::operator=(Texture&& a_other) {
	m_id   = a_other.m_id;
	m_name = a_other.m_name;

	a_other.m_id   = c_unused;
	a_other.m_name = nullptr;
	return *this;
}

Texture Graphics::CreateTexture(const TextureCreateInfo& a_texture) {
	uint16_t slot = c_maxTextures;
	for(uint16_t i = 0; i < c_maxTextures; ++i) {
		if(!g_used[i]) {
			slot = i;
			break;
		}
	}
	if(slot == c_maxTextures) { Core::Log::Fail("Ran out of available textures"); }

	g_used[slot]  = true;
	g_names[slot] = a_texture.Name;
	Texture result{slot, &g_names[slot]};
	return result;
}

void TexturePool::Init() {
	g_used.reset();
}

void TexturePool::Shutdown() {}
