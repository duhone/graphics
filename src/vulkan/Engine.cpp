#include "Graphics/Engine.h"

#include "CommandPool.h"
#include "Commands.h"
#include "EngineInternal.h"
#include "SpriteManager.h"

#include "core/Log.h"
#include "core/algorithm.h"

#include "vulkan/vulkan.hpp"

#include <exception>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

using namespace CR;
using namespace CR::Core;
using namespace CR::Graphics;
using namespace std;
using namespace std::string_literals;
using namespace glm;

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

		void ExecutePending();

		// private: internal so private anyway
		vk::Instance m_Instance;
		vk::Device m_Device;
		int32_t m_GraphicsQueueIndex{-1};
		int32_t m_TransferQueueIndex{-1};
		int32_t m_PresentationQueueIndex{-1};
		vk::Queue m_GraphicsQueue;
		vk::Queue m_TransferQueue;
		vk::Queue m_PresentationQueue;

		vk::SurfaceKHR m_PrimarySurface;
		vk::SwapchainKHR m_PrimarySwapChain;
		std::vector<vk::Image> m_PrimarySwapChainImages;
		std::vector<vk::ImageView> m_primarySwapChainImageViews;
		std::vector<vk::Framebuffer> m_frameBuffers;
		vk::RenderPass m_RenderPass;          // only 1 currently, and only 1 subpass to go with it
		vk::Semaphore m_renderingFinished;    // need to block presenting until all rendering has completed
		vk::Fence m_frameFence;

		uint32_t DeviceMemoryIndex{numeric_limits<uint32_t>::max()};
		uint32_t HostMemoryIndex{numeric_limits<uint32_t>::max()};

		ivec2 m_WindowSize{0, 0};
		std::optional<glm::vec4> m_clearColor;

		std::unique_ptr<CommandPool> m_commandPool;

		SpriteManager m_spriteManager;

		// Per frame members
		uint32_t m_currentFrameBuffer{0};
		std::unique_ptr<CommandBuffer> m_commandBuffer;

		std::vector<std::function<void()>> m_nextFrameFuncs;
	};

	unique_ptr<Engine>& GetEngine() {
		static unique_ptr<Engine> engine;
		return engine;
	}
}    // namespace

Engine::Engine(const EngineSettings& a_settings) : m_clearColor(a_settings.ClearColor) {
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
	createInfo.enabledExtensionCount   = a_settings.ExtensionsToEnableCount;
	createInfo.ppEnabledExtensionNames = a_settings.ExtensionsToEnable;

	m_Instance = vk::createInstance(createInfo);

	Log::Assert(a_settings.HInstance != nullptr, "Hinstance is required, headless mode not currently supported");
	Log::Assert(a_settings.Hwnd != nullptr, "Hwnd is required, headless mode not currently supported");

	vk::Win32SurfaceCreateInfoKHR win32Surface;
	win32Surface.hinstance = (HINSTANCE)a_settings.HInstance;
	win32Surface.hwnd      = (HWND)a_settings.Hwnd;

	m_PrimarySurface = m_Instance.createWin32SurfaceKHR(win32Surface);

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
		vector<uint32_t> graphicsQueues;
		vector<uint32_t> transferQueues;
		vector<uint32_t> presentationQueues;
		optional<uint32_t> dedicatedTransfer;
		optional<uint32_t> graphicsAndPresentation;
		for(uint32_t i = 0; i < queueProps.size(); ++i) {
			bool supportsGraphics     = false;
			bool supportsCompute      = false;
			bool supportsTransfer     = false;
			bool supportsPresentation = false;

			Log::Info("Queue family: {}", i);
			// This one should only be false for tesla compute cards and similiar
			if((queueProps[i].queueFlags & vk::QueueFlagBits::eGraphics) && queueProps[i].queueCount >= 1) {
				supportsGraphics = true;
				graphicsQueues.push_back(i);
				Log::Info("  supports graphics");
			}
			if((queueProps[i].queueFlags & vk::QueueFlagBits::eCompute) && queueProps[i].queueCount >= 1) {
				supportsCompute = true;
				Log::Info("  supports compute");
			}
			if((queueProps[i].queueFlags & vk::QueueFlagBits::eTransfer) && queueProps[i].queueCount >= 1) {
				supportsTransfer = true;
				transferQueues.push_back(i);
				Log::Info("  supports transfer");
			}
			if(device.getSurfaceSupportKHR(i, m_PrimarySurface)) {
				supportsPresentation = true;
				presentationQueues.push_back(i);
				Log::Info("  supports presentation");
			}

			// for transfers, prefer a dedicated transfer queue(probably doesnt matter). If more than one, grab first
			if(!supportsGraphics && !supportsCompute && !supportsPresentation && supportsTransfer &&
			   !dedicatedTransfer.has_value()) {
				dedicatedTransfer = i;
			}
			// For graphics, we prefer one that does both graphics and presentation
			if(supportsGraphics && supportsPresentation && !graphicsAndPresentation.has_value()) {
				graphicsAndPresentation = i;
			}
		}

		if(dedicatedTransfer.has_value()) {
			m_TransferQueueIndex = dedicatedTransfer.value();
		} else if(!transferQueues.empty()) {
			m_TransferQueueIndex = transferQueues[0];
		} else {
			Log::Info("Could not find a valid vulkan transfer queue");
		}

		if(graphicsAndPresentation.has_value()) {
			m_GraphicsQueueIndex = m_PresentationQueueIndex = graphicsAndPresentation.value();
		} else if(!graphicsQueues.empty() && !presentationQueues.empty()) {
			m_GraphicsQueueIndex     = graphicsQueues[0];
			m_PresentationQueueIndex = presentationQueues[0];
		} else {
			if(graphicsQueues.empty()) { Log::Info("Could not find a valid vulkan graphics queue"); }
			if(presentationQueues.empty()) { Log::Info("Could not find a valid presentation queue"); }
		}

		Log::Info("graphics queue family: {} transfer queue index: {} presentation queue index: {}",
		          m_GraphicsQueueIndex, m_TransferQueueIndex, m_PresentationQueueIndex);
		auto features = device.getFeatures();

		// TODO: We dont have a good heuristic for selecting a device, for now just take first one that supports
		// graphics and hope for the best.  My machine has only one, so cant test a better implementation.
		if((m_GraphicsQueueIndex != -1) && (m_TransferQueueIndex != -1) && (m_PresentationQueueIndex != -1) &&
		   features.textureCompressionBC && features.fullDrawIndexUint32) {
			foundDevice    = true;
			selectedDevice = device;
			break;
		} else {
			m_GraphicsQueueIndex     = -1;
			m_TransferQueueIndex     = -1;
			m_PresentationQueueIndex = -1;
		}
	}
	if(!foundDevice) { Log::Fail("Could not find a valid vulkan graphics device"); }

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

		if(((heapFlags & vk::MemoryPropertyFlagBits::eHostVisible) &&
		    (heapFlags & vk::MemoryPropertyFlagBits::eHostCoherent)) &&
		   (HostMemoryIndex == numeric_limits<uint32_t>::max())) {
			HostMemoryIndex = i;
			printHeapInfo();
		}
	}

	vk::PhysicalDeviceFeatures requiredFeatures;
	requiredFeatures.textureCompressionBC = true;
	requiredFeatures.fullDrawIndexUint32  = true;

	int32_t graphicsQueueIndex     = 0;
	int32_t presentationQueueIndex = 0;
	int32_t transferQueueIndex     = 0;
	int32_t queueIndexMap[256];    // surely no more than 256 queue families for all time
	fill(queueIndexMap, -1);

	float graphicsPriority = 1.0f;
	float transferPriority = 0.0f;
	vk::DeviceQueueCreateInfo queueInfo;
	std::vector<vk::DeviceQueueCreateInfo> queueInfos;
	queueInfo.queueFamilyIndex = m_GraphicsQueueIndex;
	queueInfo.queueCount       = 1;
	queueInfo.pQueuePriorities = &graphicsPriority;
	queueInfos.push_back(queueInfo);
	++queueIndexMap[m_GraphicsQueueIndex];
	graphicsQueueIndex = queueIndexMap[m_GraphicsQueueIndex];
	if(m_GraphicsQueueIndex != m_PresentationQueueIndex) {
		queueInfo.queueFamilyIndex = m_PresentationQueueIndex;
		queueInfos.push_back(queueInfo);
		++queueIndexMap[m_PresentationQueueIndex];
	}
	presentationQueueIndex     = queueIndexMap[m_PresentationQueueIndex];
	queueInfo.queueFamilyIndex = m_TransferQueueIndex;
	queueInfo.pQueuePriorities = &transferPriority;
	queueInfos.push_back(queueInfo);
	++queueIndexMap[m_TransferQueueIndex];
	transferQueueIndex = queueIndexMap[m_TransferQueueIndex];

	vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

	vk::DeviceCreateInfo createLogDevInfo;
	createLogDevInfo.queueCreateInfoCount    = (int)size(queueInfos);
	createLogDevInfo.pQueueCreateInfos       = data(queueInfos);
	createLogDevInfo.pEnabledFeatures        = &requiredFeatures;
	createLogDevInfo.enabledLayerCount       = 0;
	createLogDevInfo.ppEnabledLayerNames     = nullptr;
	createLogDevInfo.enabledExtensionCount   = (uint32_t)size(deviceExtensions);
	createLogDevInfo.ppEnabledExtensionNames = data(deviceExtensions);

	m_Device = selectedDevice.createDevice(createLogDevInfo);

	m_GraphicsQueue     = m_Device.getQueue(m_GraphicsQueueIndex, graphicsQueueIndex);
	m_PresentationQueue = m_Device.getQueue(m_PresentationQueueIndex, presentationQueueIndex);
	m_TransferQueue     = m_Device.getQueue(m_TransferQueueIndex, transferQueueIndex);

	auto surfaceCaps = selectedDevice.getSurfaceCapabilitiesKHR(m_PrimarySurface);
	Log::Info("current surface resolution: {}x{}", surfaceCaps.maxImageExtent.width, surfaceCaps.maxImageExtent.height);
	Log::Info("Min image count: {} Max image count: {}", surfaceCaps.minImageCount, surfaceCaps.maxImageCount);

	auto surfaceFormats = selectedDevice.getSurfaceFormatsKHR(m_PrimarySurface);
	Log::Info("Supported surface formats:");
	for(const auto& format : surfaceFormats) {
		Log::Info("    Format: {} ColorSpace {}", to_string(format.format), to_string(format.colorSpace));
	}

	auto presentModes = selectedDevice.getSurfacePresentModesKHR(m_PrimarySurface);
	Log::Info("Presentation modes:");
	for(const auto& mode : presentModes) { Log::Info("    Presentation Mode: {}", to_string(mode)); }

	vk::SwapchainCreateInfoKHR swapCreateInfo;
	swapCreateInfo.setClipped(true);
	swapCreateInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
	swapCreateInfo.setImageColorSpace(vk::ColorSpaceKHR::eSrgbNonlinear);
	swapCreateInfo.setImageExtent(surfaceCaps.maxImageExtent);
	swapCreateInfo.setImageFormat(vk::Format::eB8G8R8A8Srgb);
	if(m_GraphicsQueueIndex == m_PresentationQueueIndex) {
		swapCreateInfo.setImageSharingMode(vk::SharingMode::eExclusive);
		swapCreateInfo.setQueueFamilyIndexCount(0);
		swapCreateInfo.setPQueueFamilyIndices(nullptr);

	} else {
		swapCreateInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
		uint32_t queueFamilyIndices[] = {(uint32_t)m_GraphicsQueueIndex, (uint32_t)m_PresentationQueueIndex};
		swapCreateInfo.setQueueFamilyIndexCount((uint32_t)size(queueFamilyIndices));
		swapCreateInfo.setPQueueFamilyIndices(data(queueFamilyIndices));
	}
	swapCreateInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
	swapCreateInfo.setMinImageCount(2);
	swapCreateInfo.setPresentMode(vk::PresentModeKHR::eFifo);
	swapCreateInfo.setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity);
	swapCreateInfo.setSurface(m_PrimarySurface);
	swapCreateInfo.setImageArrayLayers(1);

	m_PrimarySwapChain       = m_Device.createSwapchainKHR(swapCreateInfo);
	m_PrimarySwapChainImages = m_Device.getSwapchainImagesKHR(m_PrimarySwapChain);
	for(const auto& image : m_PrimarySwapChainImages) {
		vk::ImageViewCreateInfo viewInfo;
		viewInfo.setFormat(vk::Format::eB8G8R8A8Srgb);
		viewInfo.setImage(image);
		viewInfo.setViewType(vk::ImageViewType::e2D);
		viewInfo.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount     = 1;
		viewInfo.subresourceRange.baseMipLevel   = 0;
		viewInfo.subresourceRange.levelCount     = 1;
		m_primarySwapChainImageViews.push_back(m_Device.createImageView(viewInfo));
	}
	m_WindowSize = ivec2(surfaceCaps.maxImageExtent.width, surfaceCaps.maxImageExtent.height);

	vk::AttachmentDescription attatchDesc;
	attatchDesc.initialLayout = vk::ImageLayout::eUndefined;
	attatchDesc.finalLayout   = vk::ImageLayout::ePresentSrcKHR;
	attatchDesc.format        = vk::Format::eB8G8R8A8Srgb;
	if(a_settings.ClearColor.has_value()) {
		attatchDesc.loadOp = vk::AttachmentLoadOp::eClear;
	} else {
		attatchDesc.loadOp = vk::AttachmentLoadOp::eDontCare;
	}
	attatchDesc.storeOp        = vk::AttachmentStoreOp::eStore;
	attatchDesc.samples        = vk::SampleCountFlagBits::e1;
	attatchDesc.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
	attatchDesc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

	vk::AttachmentReference attachRef;
	attachRef.attachment = 0;
	attachRef.layout     = vk::ImageLayout::eColorAttachmentOptimal;

	vk::SubpassDescription subpassDesc;
	subpassDesc.pipelineBindPoint    = vk::PipelineBindPoint::eGraphics;
	subpassDesc.colorAttachmentCount = 1;
	subpassDesc.pColorAttachments    = &attachRef;

	vk::RenderPassCreateInfo renderPassInfo;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments    = &attatchDesc;
	renderPassInfo.subpassCount    = 1;
	renderPassInfo.pSubpasses      = &subpassDesc;

	m_RenderPass = m_Device.createRenderPass(renderPassInfo);

	for(auto& imageView : m_primarySwapChainImageViews) {
		vk::FramebufferCreateInfo framebufferInfo;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments    = &imageView;
		framebufferInfo.width           = m_WindowSize.x;
		framebufferInfo.height          = m_WindowSize.y;
		framebufferInfo.renderPass      = m_RenderPass;
		framebufferInfo.layers          = 1;

		m_frameBuffers.push_back(m_Device.createFramebuffer(framebufferInfo));
	}

	vk::SemaphoreCreateInfo semInfo;
	m_renderingFinished = m_Device.createSemaphore(semInfo);

	vk::FenceCreateInfo fenceInfo;
	m_frameFence = m_Device.createFence(fenceInfo);
}

Engine::~Engine() {
	m_Device.destroyFence(m_frameFence);
	m_Device.destroySemaphore(m_renderingFinished);
	for(auto& framebuffer : m_frameBuffers) { m_Device.destroyFramebuffer(framebuffer); }
	m_Device.destroyRenderPass(m_RenderPass);
	for(auto& imageView : m_primarySwapChainImageViews) { m_Device.destroyImageView(imageView); }
	m_primarySwapChainImageViews.clear();
	m_Device.destroySwapchainKHR(m_PrimarySwapChain);
	m_Device.destroy();
	m_Instance.destroySurfaceKHR(m_PrimarySurface);
	m_Instance.destroy();
}

void Engine::ExecutePending() {
	for(auto& func : m_nextFrameFuncs) { func(); }
}

void Graphics::CreateEngine(const EngineSettings& a_settings) {
	assert(!GetEngine().get());
	GetEngine()                = make_unique<Engine>(a_settings);
	GetEngine()->m_commandPool = CreateCommandPool(CommandPool::PoolType::Primary);
}

void Graphics::Frame() {
	assert(GetEngine().get());
	auto engine = GetEngine().get();

	engine->ExecutePending();

	engine->m_currentFrameBuffer =
	    engine->m_Device
	        .acquireNextImageKHR(engine->m_PrimarySwapChain, UINT64_MAX, vk::Semaphore{}, engine->m_frameFence)
	        .value;

	engine->m_Device.waitForFences(1, &engine->m_frameFence, true, UINT64_MAX);
	engine->m_Device.resetFences(1, &engine->m_frameFence);

	engine->m_commandBuffer = engine->m_commandPool->CreateCommandBuffer();

	engine->m_commandBuffer->Begin();
	Commands::RenderPassBegin(*engine->m_commandBuffer.get(), engine->m_clearColor);
	engine->m_spriteManager.Draw(*engine->m_commandBuffer.get());
	Commands::RenderPassEnd(*engine->m_commandBuffer.get());
	engine->m_commandBuffer->End();

	vk::SubmitInfo subInfo;
	subInfo.commandBufferCount   = 1;
	subInfo.pCommandBuffers      = (vk::CommandBuffer*)engine->m_commandBuffer->GetHandle();
	subInfo.waitSemaphoreCount   = 0;
	subInfo.signalSemaphoreCount = 1;
	subInfo.pSignalSemaphores    = &engine->m_renderingFinished;
	engine->m_GraphicsQueue.submit(subInfo, vk::Fence{});

	vk::PresentInfoKHR presInfo;
	presInfo.waitSemaphoreCount = 1;
	presInfo.pWaitSemaphores    = &engine->m_renderingFinished;
	presInfo.swapchainCount     = 1;
	presInfo.pSwapchains        = &engine->m_PrimarySwapChain;
	presInfo.pImageIndices      = &engine->m_currentFrameBuffer;
	engine->m_PresentationQueue.presentKHR(presInfo);

	// Don't allow gpu to get behind, sacrifice performance for minimal latency.
	engine->m_Device.waitIdle();
}

void Graphics::ShutdownEngine() {
	assert(GetEngine().get());
	GetEngine()->m_Device.waitIdle();
	GetEngine()->ExecutePending();
	GetEngine()->m_commandBuffer.reset();
	GetEngine()->m_commandPool.reset();
	GetEngine().reset();
}

vk::Device& Graphics::GetDevice() {
	assert(GetEngine().get());
	return GetEngine()->m_Device;
}

uint32_t Graphics::GetDeviceMemoryIndex() {
	assert(GetEngine().get());
	return GetEngine()->DeviceMemoryIndex;
}

uint32_t Graphics::GetHostMemoryIndex() {
	assert(GetEngine().get());
	return GetEngine()->HostMemoryIndex;
}

uint32_t Graphics::GetGraphicsQueueIndex() {
	assert(GetEngine().get());
	return GetEngine()->m_GraphicsQueueIndex;
}

uint32_t Graphics::GetTransferQueueIndex() {
	assert(GetEngine().get());
	return GetEngine()->m_TransferQueueIndex;
}

const glm::ivec2& Graphics::GetWindowSize() {
	assert(GetEngine().get());
	return GetEngine()->m_WindowSize;
}

const vk::RenderPass& Graphics::GetRenderPass() {
	assert(GetEngine().get());
	return GetEngine()->m_RenderPass;
}

const vk::Framebuffer& Graphics::GetFrameBuffer() {
	assert(GetEngine().get());
	return GetEngine()->m_frameBuffers[GetEngine()->m_currentFrameBuffer];
}

SpriteManager& Graphics::GetSpriteManager() {
	assert(GetEngine().get());
	return GetEngine()->m_spriteManager;
}

void Graphics::ExecuteNextFrame(std::function<void()> a_func) {
	assert(GetEngine().get());
	GetEngine()->m_nextFrameFuncs.push_back(move(a_func));
}
