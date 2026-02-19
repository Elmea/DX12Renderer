#pragma once
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <string>

class Context
{
public:


public:
	void Init(int winSizeX, int winSizeY, std::string name);
	void Destroy();

	void StartFrame();

	bool ShouldClose();

private:

	GLFWwindow* window;

	int sizeX, sizeY;
};