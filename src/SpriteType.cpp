#include "Graphics/SpriteType.h"
#include "SpriteTypeImpl.h"

#include "core/Log.h"
#include "core/literals.h"

using namespace std;
using namespace CR;
using namespace CR::Core;
using namespace CR::Graphics;
using namespace CR::Core::Literals;

SpriteTypeImpl::SpriteTypeImpl(const SpriteTypeCreateInfo& a_info) :
    m_name(a_info.Name), m_textureSize(a_info.TextureSize) {
	Log::Assert(a_info.TextureSize.x < 8_Kb && a_info.TextureSize.y < 8_Kb,
	            "Texture size has a maximum size of 8Kx8K. Tried to create a texture with size {}x{}",
	            a_info.TextureSize.x, a_info.TextureSize.y);

	CreatePipelineArgs info;
	info.ShaderModule = a_info.ShaderModule;

	m_pipeline = CreatePipeline(info);
}

std::shared_ptr<Graphics::SpriteType> Graphics::CreateSpriteType(const SpriteTypeCreateInfo& a_info) {
	return make_shared<Graphics::SpriteTypeImpl>(a_info);
}