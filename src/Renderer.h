#pragma once
#include "glfwWindow/Context.h"
#include <dxgi1_6.h>

#include <wrl.h>
template <typename T>
using MComPtr = Microsoft::WRL::ComPtr<T>;

class Renderer
{
public:
	int Init();
	void Run();
	void Destroy();

private:

	Context context;
	MComPtr<IDXGIFactory6> factory;

private:

	int InitFactory();

};