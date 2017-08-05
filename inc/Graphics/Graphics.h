#pragma once

namespace CR::Graphics {
	class Engine {
	public:
		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;
	protected:
		Engine() = default;
		virtual ~Engine() = default;
	};

	Engine* GetEngine();
}