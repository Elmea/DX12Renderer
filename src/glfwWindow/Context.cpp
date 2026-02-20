#include "DebugMacros.h"

#include "Context.h"

int Context::Init(uint32_t winSizeX, uint32_t winSizeY, std::string name)
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(winSizeX, winSizeY, name.c_str(), nullptr, nullptr);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // GLFW_CURSOR_DISABLED);

	if (!window)
	{
		DEBUG_LOG("Window cant be initialized", LOGTYPE_ERROR);
		return 1;
	}
	else
	{
		DEBUG_LOG("Window initialized", LOGTYPE_VALIDATION);
		return 0;
	}
}

void Context::Destroy()
{
	glfwDestroyWindow(window);
	window = nullptr;
	glfwTerminate();
}

void Context::StartFrame()
{
	glfwPollEvents();
}

bool Context::ShouldClose()
{
	return glfwWindowShouldClose(window);
}

void Context::GetWindowSize(int* width, int* height)
{
	glfwGetWindowSize(window, width, height);
}
