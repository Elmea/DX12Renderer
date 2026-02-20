#pragma once
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "ElmeasMaths/RedfoxMaths.hpp"

#include <string>

class Context
{
public:


public:
	int Init(int winSizeX, int winSizeY, std::string name);
	void Destroy();

	void StartFrame();

	bool ShouldClose();

	RedFoxMaths::Float2 GetWindowSize();

private:

	GLFWwindow* window;

	int sizeX, sizeY;
};