#pragma once
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <string>

class Context
{
public:
	GLFWwindow* window;

public:
	int Init(uint32_t winSizeX, uint32_t winSizeY, std::string name);
	void Destroy();

	void StartFrame();

	bool ShouldClose();

	void GetWindowSize(int* width, int* height);

private:

};