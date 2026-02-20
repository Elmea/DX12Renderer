#pragma once
#include "glfwWindow/Context.h"
#include <dxgi1_6.h>
#include <d3d12.h> 
#include <array>

#include <wrl.h>
template <typename T>
using MComPtr = Microsoft::WRL::ComPtr<T>;

const DXGI_FORMAT sceneColorFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
constexpr float sceneClearColor[] = { 0.0f, 0.1f, 0.2f, 1.0f };

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

	// Use Swapchain backbuffer texture as color output.
	MComPtr<ID3D12DescriptorHeap> sceneRTViewHeap; 

	// = Depth =
	const DXGI_FORMAT sceneDepthFormat = DXGI_FORMAT_D16_UNORM;
	const D3D12_CLEAR_VALUE sceneDepthClearValue{ .Format = sceneDepthFormat, .DepthStencil = { 1.0f, 0 } };
	MComPtr<ID3D12Resource> sceneDepthTexture;
	MComPtr<ID3D12DescriptorHeap> sceneDepthRTViewHeap;

private:

	int InitFactory();
	int InitDevice();
	int InitSwapchain();
	int InitCommands();
	int InitSceneTextures();

	void DestroyFactory();
	void DestroyDevice();
	void DestroySwapchain();
	void DestroyCommands();
	void DestroySceneTextures();
};