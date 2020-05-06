#include "Graphics/SpriteType.h"
#include "SpriteManager.h"
#include "SpriteTypeImpl.h"
#include "vulkan/EngineInternal.h"

#include "core/Log.h"
#include "core/literals.h"

#include "shaders/Simple.h"

using namespace std;
using namespace CR;
using namespace CR::Core;
using namespace CR::Graphics;
using namespace CR::Core::Literals;

SpriteTypeImpl::SpriteTypeImpl(uint8_t a_index) : m_index(a_index) {}
SpriteTypeImpl::~SpriteTypeImpl() {
	GetSpriteManager().FreeType(m_index);
}

std::shared_ptr<Graphics::SpriteType> Graphics::CreateSpriteType() {
	CreatePipelineArgs info;
	info.ShaderModule = CR::embed::GetSimple();

	Pipeline pipeline{info};
	auto typeIndex = GetSpriteManager().CreateType("simple", move(pipeline));
	return make_shared<Graphics::SpriteTypeImpl>(typeIndex);
}
