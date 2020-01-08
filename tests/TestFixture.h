#pragma once

#include "Graphics/Engine.h"

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

class TestFixture {
  protected:
	GLFWwindow* Window{nullptr};

  public:
	TestFixture() {
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		Window = glfwCreateWindow(1280, 720, "Vulkan window", nullptr, nullptr);

		CR::Graphics::EngineSettings settings;
		settings.ApplicationName    = "Unit Test";
		settings.ApplicationVersion = 1;
		settings.EnableDebug        = true;
		settings.ExtensionsToEnable = glfwGetRequiredInstanceExtensions(&settings.ExtensionsToEnableCount);
		settings.Hwnd               = glfwGetWin32Window(Window);
		settings.HInstance          = GetModuleHandle(nullptr);
		settings.ClearColor         = glm::vec4(0.0f, 0.0f, 0.75f, 1.0f);

		REQUIRE_NOTHROW(CR::Graphics::CreateEngine(settings));
	}

	~TestFixture() {
		REQUIRE_NOTHROW(CR::Graphics::ShutdownEngine());

		glfwDestroyWindow(Window);

		glfwTerminate();
	}
};
