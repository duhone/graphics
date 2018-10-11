#include "Graphics/Engine.h"
#include "EngineInternal.h"
#include "vulkan/vulkan.hpp"
#include <exception>
#include <iostream>
#include <memory>
#include <vector>

using namespace CR::Graphics;
using namespace std;
using namespace std::string_literals;

namespace {
	constexpr uint MajorVersion = 0;    // 64K max
	constexpr uint MinorVersion = 1;    // 256 max
	constexpr uint PatchVersion = 1;    // 256 max
	constexpr uint Version      = (MajorVersion << 16) || (MajorVersion << 8) || (PatchVersion);

	class Engine {
	  public:
		Engine(const EngineSettings& a_settings);
		~Engine();
		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;

		// private: internal so private anyway
		vk::Instance m_Instance;
		vk::Device m_Device;

		uint32_t DeviceMemoryIndex{numeric_limits<uint32_t>::max()};
		uint32_t HostMemoryIndex{numeric_limits<uint32_t>::max()};
	};

	unique_ptr<Engine> g_Engine;
}    // namespace

Engine::Engine(const EngineSettings& a_settings) {
	vector<string> enabledLayers;
	if(a_settings.EnableDebug) {
		vector<vk::LayerProperties> layers = vk::enumerateInstanceLayerProperties();
		for(const auto& layer : layers) {
			if("VK_LAYER_LUNARG_standard_validation"s == layer.layerName) { enabledLayers.push_back(layer.layerName); }
		}
	}

	vk::ApplicationInfo appInfo;
	appInfo.pApplicationName   = a_settings.ApplicationName.c_str();
	appInfo.applicationVersion = a_settings.ApplicationVersion;
	appInfo.pEngineName        = "Conjure";
	appInfo.engineVersion      = Version;
	appInfo.apiVersion         = VK_API_VERSION_1_0;

	vector<const char*> enabledLayersPtrs;
	for(auto& layer : enabledLayers) { enabledLayersPtrs.push_back(layer.c_str()); }

	vk::InstanceCreateInfo createInfo;
	createInfo.pApplicationInfo        = &appInfo;
	createInfo.enabledLayerCount       = (uint32_t)size(enabledLayersPtrs);
	createInfo.ppEnabledLayerNames     = data(enabledLayersPtrs);
	createInfo.enabledExtensionCount   = 0;
	createInfo.ppEnabledExtensionNames = nullptr;

	m_Instance = vk::createInstance(createInfo);

	vector<vk::PhysicalDevice> physicalDevices = m_Instance.enumeratePhysicalDevices();

	vk::PhysicalDevice selectedDevice;
	bool foundDevice       = false;
	int graphicsQueueIndex = -1;
	int transferQueueIndex = -1;
	for(auto& device : physicalDevices) {
		auto props = device.getProperties();
		cout << "Device Name: " << props.deviceName << endl;
		auto memProps = device.getMemoryProperties();
		cout << "  Device max allocations: " << props.limits.maxMemoryAllocationCount << endl;
		cout << "  Device max array layers: " << props.limits.maxImageArrayLayers << endl;
		cout << "  Device max 2D image dimensions: " << props.limits.maxImageDimension2D << endl;
		for(uint32_t i = 0; i < memProps.memoryTypeCount; ++i) {
			if(memProps.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal) {
				cout << "  Device Local Memory Amount: "
				     << memProps.memoryHeaps[memProps.memoryTypes[i].heapIndex].size / (1024 * 1024) << "MB" << endl;
			}
		}

		vector<vk::QueueFamilyProperties> queueProps = device.getQueueFamilyProperties();
		bool supportsGraphics                        = false;
		bool supportsTransfer                        = false;
		for(uint32_t i = 0; i < queueProps.size(); ++i) {
			// This one should only be false for tesla compute cards and similiar
			if((queueProps[i].queueFlags & vk::QueueFlagBits::eGraphics) && queueProps[i].queueCount >= 1) {
				supportsGraphics   = true;
				graphicsQueueIndex = i;
			}
			if((queueProps[i].queueFlags & vk::QueueFlagBits::eTransfer) && queueProps[i].queueCount >= 1) {
				supportsTransfer   = true;
				transferQueueIndex = i;
			}
		}

		auto features = device.getFeatures();

		// TODO: We dont have a good heuristic for selecting a device, for now just take first one that supports
		// graphics and hope for the best.  My machine has only one, so cant test a better implementation.
		if(supportsGraphics && supportsTransfer && features.textureCompressionBC && features.fullDrawIndexUint32) {
			foundDevice    = true;
			selectedDevice = device;
			break;
		}
	}
	if(!foundDevice) { throw runtime_error("Could not find a valid vulkan device"); }

	auto memProps = selectedDevice.getMemoryProperties();
	for(uint32_t i = 0; i < memProps.memoryHeapCount; ++i) {
		auto heapSize   = memProps.memoryHeaps[i].size / 1024 / 1024;
		auto& heapFlags = memProps.memoryHeaps[i].flags;
		if(heapFlags & vk::MemoryHeapFlagBits::eDeviceLocal) {
			cout << "Device Heap. Size " << heapSize << "MB" << endl;
		} else {
			cout << "Host Heap. Size " << heapSize << "MB" << endl;
		}
	}
	for(uint32_t i = 0; i < memProps.memoryTypeCount; ++i) {
		auto& heapIndex    = memProps.memoryTypes[i].heapIndex;
		auto& heapFlags    = memProps.memoryTypes[i].propertyFlags;
		auto printHeapInfo = [&]() {
			cout << "Device Heap: " << heapIndex << " Type Index: " << i << endl;
			if(heapFlags & vk::MemoryPropertyFlagBits::eDeviceLocal) { cout << "  Device local" << endl; }
			if(heapFlags & vk::MemoryPropertyFlagBits::eHostVisible) { cout << "  Host visible" << endl; }
			if(heapFlags & vk::MemoryPropertyFlagBits::eHostCached) { cout << "  Host cached" << endl; }
			if(heapFlags & vk::MemoryPropertyFlagBits::eHostCoherent) { cout << "  Host coherent" << endl; }
		};

		if((heapFlags & vk::MemoryPropertyFlagBits::eDeviceLocal) &&
		   (DeviceMemoryIndex == numeric_limits<uint32_t>::max())) {
			DeviceMemoryIndex = i;
			printHeapInfo();
		}

		if((heapFlags & vk::MemoryPropertyFlagBits::eHostVisible) &&
		   (HostMemoryIndex == numeric_limits<uint32_t>::max())) {
			HostMemoryIndex = i;
			printHeapInfo();
		}
	}

	vk::PhysicalDeviceFeatures requiredFeatures;
	requiredFeatures.textureCompressionBC = true;
	requiredFeatures.fullDrawIndexUint32  = true;

	float graphicsPriority = 1.0f;
	float transferPriority = 0.0f;
	vk::DeviceQueueCreateInfo queueInfos[2];
	queueInfos[0].queueFamilyIndex = graphicsQueueIndex;
	queueInfos[0].queueCount       = 1;
	queueInfos[0].pQueuePriorities = &graphicsPriority;
	queueInfos[1]                  = queueInfos[0];
	queueInfos[1].queueFamilyIndex = transferQueueIndex;
	queueInfos[1].pQueuePriorities = &transferPriority;

	vk::DeviceCreateInfo createLogDevInfo;
	createLogDevInfo.queueCreateInfoCount    = (int)size(queueInfos);
	createLogDevInfo.pQueueCreateInfos       = queueInfos;
	createLogDevInfo.pEnabledFeatures        = &requiredFeatures;
	createLogDevInfo.enabledLayerCount       = 0;
	createLogDevInfo.ppEnabledLayerNames     = nullptr;
	createLogDevInfo.enabledExtensionCount   = 0;
	createLogDevInfo.ppEnabledExtensionNames = nullptr;

	m_Device = selectedDevice.createDevice(createLogDevInfo);
}

Engine::~Engine() {
	m_Device.destroy();
	m_Instance.destroy();
}

void CR::Graphics::CreateEngine(const EngineSettings& a_settings) {
	g_Engine = make_unique<Engine>(a_settings);
}

void CR::Graphics::ShutdownEngine() {
	g_Engine.reset();
}

vk::Device& CR::Graphics::GetDevice() {
	assert(g_Engine.get());
	return g_Engine->m_Device;
}

uint32_t CR::Graphics::GetDeviceMemoryIndex() {
	assert(g_Engine.get());
	return g_Engine->DeviceMemoryIndex;
}

uint32_t CR::Graphics::GetHostMemoryIndex() {
	assert(g_Engine.get());
	return g_Engine->HostMemoryIndex;
}
