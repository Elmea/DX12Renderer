#include "DebugMacros.h"

#include "Context.h"

#define REDFOXMATHS_IMPLEMENTATION
#include "ElmeasMaths/RedfoxMaths.hpp"

int Context::Init(int winSizeX, int winSizeY, std::string name)
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(winSizeX, winSizeX, name.c_str(), nullptr, nullptr);

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

RedFoxMaths::Float2 Context::GetWindowSize()
{
	return RedFoxMaths::Float2(sizeX, sizeY);
}