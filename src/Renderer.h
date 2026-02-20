#pragma once
#include "glfwWindow/Context.h"
#include <dxgi1_6.h>
#include <d3d12.h> 
#include <array>

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
	MComPtr<ID3D12Device> device;
	MComPtr<ID3D12CommandQueue> graphicsQueue;
	MComPtr<IDXGISwapChain3> swapchain;
	MComPtr<ID3D12GraphicsCommandList1> cmdList;

	HANDLE deviceFenceEvent;
	MComPtr<ID3D12Fence> deviceFence;
	uint32_t deviceFenceValue = 1u;

	const DXGI_FORMAT sceneColorFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	static const uint32_t bufferingCount = 3;

	std::array<MComPtr<ID3D12Resource>, bufferingCount> swapchainImages{ nullptr };
	uint32_t swapchainFrameIndex = 0u;

	HANDLE swapchainFenceEvent = nullptr;
	MComPtr<ID3D12Fence> swapchainFence;
	std::array<uint32_t, bufferingCount> swapchainFenceValues{ 0u };

	std::array<MComPtr<ID3D12CommandAllocator>, bufferingCount> cmdAllocs;

private:

	int InitFactory();
	int InitDevice();
	int InitSwapchain();
	int InitCommands();

	void DestroyFactory();
	void DestroyDevice();
	void DestroySwapchain();
	void DestroyCommands();
};