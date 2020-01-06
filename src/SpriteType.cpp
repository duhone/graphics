#include "Graphics/SpriteType.h"
#include "SpriteManager.h"
#include "SpriteTypeImpl.h"
#include "vulkan/EngineInternal.h"

#include "core/Log.h"
#include "core/literals.h"

using namespace std;
using namespace CR;
using namespace CR::Core;
using namespace CR::Graphics;
using namespace CR::Core::Literals;

SpriteTypeImpl::SpriteTypeImpl(uint8_t a_index) : m_index(a_index) {}
SpriteTypeImpl::~SpriteTypeImpl() {
	GetSpriteManager().FreeType(m_index);
}

std::shared_ptr<Graphics::SpriteType> Graphics::CreateSpriteType(const SpriteTypeCreateInfo& a_info) {
	CreatePipelineArgs info;
	info.ShaderModule = a_info.ShaderModule;

	auto pipeline  = CreatePipeline(info);
	auto typeIndex = GetSpriteManager().CreateType(a_info.Name, move(pipeline));
	return make_shared<Graphics::SpriteTypeImpl>(typeIndex);
}
