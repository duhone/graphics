#include "Graphics/Sprite.h"

#include "SpriteManager.h"
#include "vulkan/EngineInternal.h"

namespace {
	class SpriteImpl : public CR::Graphics::Sprite {
	  public:
		SpriteImpl(uint16_t a_index) : m_index(a_index) {}
		virtual ~SpriteImpl();
		SpriteImpl(const SpriteImpl&) = delete;
		SpriteImpl& operator=(const SpriteImpl&) = delete;

		void SetProps(const Props& a_props) override;

		uint16_t GetIndex() const { return m_index; }

	  private:
		uint16_t m_index;
	};
}    // namespace

using namespace std;
using namespace CR;
using namespace CR::Graphics;

SpriteImpl::~SpriteImpl() {
	GetSpriteManager().FreeSprite(m_index);
}

void SpriteImpl::SetProps(const Props& a_props) {
	GetSpriteManager().SetSprite(m_index, a_props.Position, a_props.Color);
}

std::unique_ptr<Graphics::Sprite> Graphics::CreateSprite(const SpriteCreateInfo& a_info) {
	uint16_t index = GetSpriteManager().CreateSprite(a_info.Name, a_info.Template);
	return make_unique<SpriteImpl>(index);
}
