#include "Macros.h"

#include "Context.h"

void Context::Init(int winSizeX, int winSizeY, std::string name)
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(winSizeX, winSizeX, name.c_str(), nullptr, nullptr);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!window)
	{
		DEBUG_LOG("Window cant be initialized", LOGTYPE_ERROR);
	}
	else
	{
		DEBUG_LOG("Window initialized", LOGTYPE_VALIDATION);
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