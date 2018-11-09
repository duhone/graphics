#include "Graphics/Engine.h"
#include "EngineInternal.h"
#include "core/Log.h"
#include "vulkan/vulkan.hpp"
#include <exception>
#include <iostream>
#include <memory>
#include <vector>

using namespace CR::Core;
using namespace CR::Graphics;
using namespace std;
using namespace std::string_literals;

namespace {
	constexpr uint32_t MajorVersion = 0;    // 64K max
	constexpr uint32_t MinorVersion = 1;    // 256 max
	constexpr uint32_t PatchVersion = 1;    // 256 max
	constexpr uint32_t Version      = (MajorVersion << 16) || (MajorVersion << 8) || (PatchVersion);

	class Engine {
	  public:
		Engine(const EngineSettings& a_settings);
		~Engine();
		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;

		// private: internal so private anyway
		vk::Instance m_Instance;
		vk::Device m_Device;
		int32_t m_GraphicsQueueIndex{-1};
		int32_t m_TransferQueueIndex{-1};
		vk::Queue m_GraphicsQueue;
		vk::Queue m_TransferQueue;

		uint32_t DeviceMemoryIndex{numeric_limits<uint32_t>::max()};
		uint32_t HostMemoryIndex{numeric_limits<uint32_t>::max()};
	};

	unique_ptr<Engine>& GetEngine() {
		static unique_ptr<Engine> engine;
		return engine;
	}
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
	bool foundDevice = false;
	for(auto& device : physicalDevices) {
		auto props = device.getProperties();
		Log::Info("Device Name : {}", props.deviceName);
		auto memProps = device.getMemoryProperties();
		Log::Info("  Device max allocations: {}", props.limits.maxMemoryAllocationCount);
		Log::Info("  Device max array layers: {}", props.limits.maxImageArrayLayers);
		Log::Info("  Device max 2D image dimensions: {}", props.limits.maxImageDimension2D);
		for(uint32_t i = 0; i < memProps.memoryTypeCount; ++i) {
			if(memProps.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal) {
				Log::Info("  Device Local Memory Amount: {}MB",
				          memProps.memoryHeaps[memProps.memoryTypes[i].heapIndex].size / (1024 * 1024));
			}
		}

		vector<vk::QueueFamilyProperties> queueProps = device.getQueueFamilyProperties();
		for(uint32_t i = 0; i < queueProps.size(); ++i) {
			bool supportsGraphics = false;
			bool supportsTransfer = false;

			Log::Info("Queue family: {}", i);
			// This one should only be false for tesla compute cards and similiar
			if((queueProps[i].queueFlags & vk::QueueFlagBits::eGraphics) && queueProps[i].queueCount >= 1) {
				supportsGraphics = true;
				Log::Info("  supports graphics");
			}
			if((queueProps[i].queueFlags & vk::QueueFlagBits::eCompute) && queueProps[i].queueCount >= 1) {
				Log::Info("  supports compute");
			}
			if((queueProps[i].queueFlags & vk::QueueFlagBits::eTransfer) && queueProps[i].queueCount >= 1) {
				supportsTransfer = true;
				Log::Info("  supports transfer");
			}
			// for transfers, prefer a dedicated transfer queue(probably doesnt matter)
			if(!supportsGraphics && supportsTransfer) { m_TransferQueueIndex = i; }
			if(supportsGraphics) {
				m_GraphicsQueueIndex = i;
				if(supportsTransfer && (m_TransferQueueIndex == -1)) { m_TransferQueueIndex = i; }
			}
		}
		Log::Info("graphics queue family: {} transfer queue index: {}", m_GraphicsQueueIndex, m_TransferQueueIndex);
		auto features = device.getFeatures();

		// TODO: We dont have a good heuristic for selecting a device, for now just take first one that supports
		// graphics and hope for the best.  My machine has only one, so cant test a better implementation.
		if((m_GraphicsQueueIndex != -1) && (m_TransferQueueIndex != -1) && features.textureCompressionBC &&
		   features.fullDrawIndexUint32) {
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
			Log::Info("Device Heap. Size {}MB", heapSize);
		} else {
			Log::Info("Host Heap. Size {}MB", heapSize);
		}
	}
	for(uint32_t i = 0; i < memProps.memoryTypeCount; ++i) {
		auto& heapIndex    = memProps.memoryTypes[i].heapIndex;
		auto& heapFlags    = memProps.memoryTypes[i].propertyFlags;
		auto printHeapInfo = [&]() {
			Log::Info("Device Heap:  {} Type Index: {}", heapIndex, i);
			if(heapFlags & vk::MemoryPropertyFlagBits::eDeviceLocal) { Log::Info("  Device local"); }
			if(heapFlags & vk::MemoryPropertyFlagBits::eHostVisible) { Log::Info("  Host visible"); }
			if(heapFlags & vk::MemoryPropertyFlagBits::eHostCached) { Log::Info("  Host cached"); }
			if(heapFlags & vk::MemoryPropertyFlagBits::eHostCoherent) { Log::Info("  Host coherent"); }
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
	queueInfos[0].queueFamilyIndex = m_GraphicsQueueIndex;
	queueInfos[0].queueCount       = 1;
	queueInfos[0].pQueuePriorities = &graphicsPriority;
	queueInfos[1]                  = queueInfos[0];
	queueInfos[1].queueFamilyIndex = m_TransferQueueIndex;
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

	m_GraphicsQueue = m_Device.getQueue(m_GraphicsQueueIndex, 0);
	m_TransferQueue = m_Device.getQueue(m_TransferQueueIndex, m_GraphicsQueueIndex == m_TransferQueueIndex ? 1 : 0);
}

Engine::~Engine() {
	m_Device.destroy();
	m_Instance.destroy();
}

void CR::Graphics::CreateEngine(const EngineSettings& a_settings) {
	assert(!GetEngine().get());
	GetEngine() = make_unique<Engine>(a_settings);
}

void CR::Graphics::ShutdownEngine() {
	assert(GetEngine().get());
	GetEngine().reset();
}

vk::Device& CR::Graphics::GetDevice() {
	assert(GetEngine().get());
	return GetEngine()->m_Device;
}

uint32_t CR::Graphics::GetDeviceMemoryIndex() {
	assert(GetEngine().get());
	return GetEngine()->DeviceMemoryIndex;
}

uint32_t CR::Graphics::GetHostMemoryIndex() {
	assert(GetEngine().get());
	return GetEngine()->HostMemoryIndex;
}

uint32_t CR::Graphics::GetGraphicsQueueIndex() {
	assert(GetEngine().get());
	return GetEngine()->m_GraphicsQueueIndex;
}

uint32_t CR::Graphics::GetTransferQueueIndex() {
	assert(GetEngine().get());
	return GetEngine()->m_TransferQueueIndex;
}

void CR::Graphics::SubmitGraphicsCommands(const std::vector<vk::CommandBuffer>& cmds) {
	assert(GetEngine().get());
	vk::SubmitInfo submit;
	submit.commandBufferCount = (uint32_t)cmds.size();
	submit.pCommandBuffers    = cmds.data();
	GetEngine()->m_GraphicsQueue.submit(1, &submit, vk::Fence{});
}

void CR::Graphics::SubmitTransferCommands(const std::vector<vk::CommandBuffer>& cmds) {
	assert(GetEngine().get());
	vk::SubmitInfo submit;
	submit.commandBufferCount = (uint32_t)cmds.size();
	submit.pCommandBuffers    = cmds.data();
	GetEngine()->m_TransferQueue.submit(1, &submit, vk::Fence{});
}
