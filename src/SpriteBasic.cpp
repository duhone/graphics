#include "Graphics/SpriteBasic.h"

#include "EngineInternal.h"
#include "SpriteManagerBasic.h"

namespace {
	class SpriteBasicImpl : public CR::Graphics::SpriteBasic {
	  public:
		SpriteBasicImpl(uint16_t a_index) : m_index(a_index) {}
		virtual ~SpriteBasicImpl();
		SpriteBasicImpl(const SpriteBasicImpl&) = delete;
		SpriteBasicImpl& operator=(const SpriteBasicImpl&) = delete;

		void SetProps(const Props& a_props) override;

		uint16_t GetIndex() const { return m_index; }

	  private:
		uint16_t m_index;
	};
}    // namespace

using namespace std;
using namespace CR;
using namespace CR::Graphics;

SpriteBasicImpl::~SpriteBasicImpl() {
	GetSpriteManagerBasic().FreeSprite(m_index);
}

void SpriteBasicImpl::SetProps(const Props& a_props) {
	GetSpriteManagerBasic().SetSprite(m_index, a_props.Position, a_props.Color, a_props.FrameRate);
}

std::unique_ptr<Graphics::SpriteBasic> Graphics::CreateSpriteBasic(const SpriteBasicCreateInfo& a_info) {
	uint16_t index = GetSpriteManagerBasic().CreateSprite(a_info.Name, a_info.Template, a_info.TextureName.c_str());
	return make_unique<SpriteBasicImpl>(index);
}
