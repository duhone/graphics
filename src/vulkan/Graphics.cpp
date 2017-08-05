#include "Graphics/Graphics.h"

namespace {
	class EngineImpl : public CR::Graphics::Engine {
	public:
		EngineImpl() = default;
		virtual ~EngineImpl() = default;
		EngineImpl(const EngineImpl&) = delete;
		EngineImpl& operator=(const EngineImpl&) = delete;
	};
}

using namespace CR::Graphics;

Engine* CR::Graphics::GetEngine() {
	static EngineImpl engine;
	return &engine;
}