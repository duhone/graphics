#include "Graphics/Engine.h"
#include "ErrorTranslation.h"
#include "vulkan/vulkan.h"
#include <memory>
#include <exception>

using namespace CR::Graphics;
using namespace std;

namespace {
	constexpr uint MajorVersion = 0; //64K max
	constexpr uint MinorVersion = 1; //256 max
	constexpr uint PatchVersion = 0; //256 max
	constexpr uint Version = (MajorVersion << 16) || (MajorVersion << 8) || (PatchVersion);

	class Engine {
	public:
		Engine(const EngineSettings& a_settings);
		~Engine();
		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;
	private:
		VkInstance m_Instance{nullptr};
	};

	unique_ptr<Engine> g_Engine;
}

Engine::Engine(const EngineSettings& a_settings) {
	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = a_settings.ApplicationName.c_str();
	appInfo.applicationVersion = a_settings.ApplicationVersion;
	appInfo.pEngineName = "Conjure";
	appInfo.engineVersion = Version;
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = nullptr;
	createInfo.enabledExtensionCount = 0;
	createInfo.ppEnabledExtensionNames = nullptr;

	VkResult result = vkCreateInstance(&createInfo, nullptr, &m_Instance);
	if(result != VK_SUCCESS) {
		throw runtime_error(TranslateError(result).data());
	} 
}

Engine::~Engine() {
	vkDestroyInstance(m_Instance, nullptr);
}

void CR::Graphics::CreateEngine(const EngineSettings& a_settings) {
	g_Engine = make_unique<Engine>(a_settings);
}

void CR::Graphics::ShutdownEngine() {
	g_Engine.reset();
}
