#include <DXGIDebug.h>
#include <format>

#include "Renderer.h"
#include "DebugMacros.h"

#ifdef _DEBUG
DWORD VLayerCallbackCookie = 0;

void ValidationLayersDebugCallback(D3D12_MESSAGE_CATEGORY _category,
	D3D12_MESSAGE_SEVERITY _severity,
	D3D12_MESSAGE_ID _ID,
	LPCSTR _description,
	void* _context)
{
	(void)_context;

	std::wstring categoryStr;

	switch (_category)
	{
	case D3D12_MESSAGE_CATEGORY_APPLICATION_DEFINED:
		categoryStr = L"Application Defined";
		break;
	case D3D12_MESSAGE_CATEGORY_MISCELLANEOUS:
		categoryStr = L"Miscellaneous";
		break;
	case D3D12_MESSAGE_CATEGORY_INITIALIZATION:
		categoryStr = L"Initialization";
		break;
	case D3D12_MESSAGE_CATEGORY_CLEANUP:
		categoryStr = L"Cleanup";
		break;
	case D3D12_MESSAGE_CATEGORY_COMPILATION:
		categoryStr = L"Compilation";
		break;
	case D3D12_MESSAGE_CATEGORY_STATE_CREATION:
		categoryStr = L"State Creation";
		break;
	case D3D12_MESSAGE_CATEGORY_STATE_SETTING:
		categoryStr = L"State Setting";
		break;
	case D3D12_MESSAGE_CATEGORY_STATE_GETTING:
		categoryStr = L"State Getting";
		break;
	case D3D12_MESSAGE_CATEGORY_RESOURCE_MANIPULATION:
		categoryStr = L"Resource Manipulation";
		break;
	case D3D12_MESSAGE_CATEGORY_EXECUTION:
		categoryStr = L"Execution";
		break;
	case D3D12_MESSAGE_CATEGORY_SHADER:
		categoryStr = L"Shader";
		break;
	default:
		categoryStr = L"Unknown";
		break;
	}

	switch (_severity)
	{
	case D3D12_MESSAGE_SEVERITY_CORRUPTION:
	case D3D12_MESSAGE_SEVERITY_ERROR:
		DEBUG_LOG(_description, LOGTYPE_ERROR)
		break;
	case D3D12_MESSAGE_SEVERITY_WARNING:
		DEBUG_LOG(_description, LOGTYPE_WARNING)
			break;
	case D3D12_MESSAGE_SEVERITY_INFO:
		// Filter Info: too much logging on Resource create/destroy and Swapchain Present.
		return;
	case D3D12_MESSAGE_SEVERITY_MESSAGE:
	default:
		DEBUG_LOG(_description, LOGTYPE_LOG)
		break;
	}
}
#endif

#define REDFOXMATHS_IMPLEMENTATION
#include "ElmeasMaths/RedfoxMaths.hpp"

#pragma region Initialisations
int Renderer::Init()
{
	DEBUG_LOG("Initializing...", LOGTYPE_INFO)

	int initializationResult = 0;
#define INITRES(func) if(!initializationResult) initializationResult = initializationResult || func;

	INITRES(context.Init(1200, 800, "DX12 Renderer"));
	INITRES(InitFactory());
	INITRES(InitDevice());
	INITRES(InitSwapchain());
	INITRES(InitCommands());
	INITRES(InitSceneTextures());

	return initializationResult;
}

int Renderer::InitFactory()
{
	DEBUG_LOG("Initializing Factory...", LOGTYPE_LOG)

	UINT dxgiFactoryFlags = 0;

#ifdef _DEBUG
	DEBUG_LOG("Debug detected, initialiazing Validation layer...", LOGTYPE_LOG)

	// Validation Layer
	{
		// Debug controller
		{
			MComPtr<ID3D12Debug1> debugController = nullptr;

			const HRESULT hrDebugInterface = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
			if (SUCCEEDED(hrDebugInterface))
			{
				debugController->EnableDebugLayer();
				debugController->SetEnableGPUBasedValidation(true);
				DEBUG_LOG("Validation layer DebugController initialization succeded.", LOGTYPE_LOG)
			}
			else
			{
				DEBUG_LOG("Validation layer DebugController initialization failed.", LOGTYPE_WARNING)
			}
		}

		// Report live objects
		{
			MComPtr<IDXGIInfoQueue> dxgiInfoQueue = nullptr;

			const HRESULT hrDebugInterface = DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue));
			if (SUCCEEDED(hrDebugInterface))
			{
				/**
				* Enable this to trigger breakpoints on ReportLiveObjects() and have errors in VisualStudio's output window.
				* WARNING: ReportLiveObjects() will ONLY output in VisualStudio's output window and not exe CONSOLE.
				*/
				dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
				dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
				dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING, true);

				DEBUG_LOG("Validation layer DebugInfoQueue uninitialized succeded.", LOGTYPE_LOG);
			}
			else
			{
				DEBUG_LOG("Validation layer DebugInfoQueue uninitialized failed.", LOGTYPE_WARNING);
			}
		}
	}

	// Enable additional debug layers.
	dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	const HRESULT hrFactoryCreated = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory));
	if (FAILED(hrFactoryCreated))
	{
		DEBUG_LOG("Create Factory failed!", LOGTYPE_ERROR);
		return EXIT_FAILURE;
	}
	else
	{
		DEBUG_LOG("Create Factory sucess!", LOGTYPE_VALIDATION);
		return EXIT_SUCCESS;
	}
}

int Renderer::InitDevice()
{
	DEBUG_LOG("Initializing Device...", LOGTYPE_LOG)

	MComPtr<IDXGIAdapter3> adapter;

	const HRESULT hrQueryGPU = factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter));
	if (FAILED(hrQueryGPU))
	{
		DEBUG_LOG("NO adaptater found!", LOGTYPE_ERROR);
		return EXIT_FAILURE;
	}

	const HRESULT hrDeviceCreated = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
	if (FAILED(hrDeviceCreated))
	{
		DEBUG_LOG("Device creation failed!", LOGTYPE_ERROR);
		return EXIT_FAILURE;
	}
	else
	{
		const LPCWSTR name = L"Main Device";
		device->SetName(name);
		DEBUG_LOG("Device creation succeded!", LOGTYPE_VALIDATION);
	}

#ifdef _DEBUG
	MComPtr<ID3D12InfoQueue1> infoQueue = nullptr;
	DEBUG_LOG("Debug detected, initialiazing Validation layer...", LOGTYPE_LOG)

	const HRESULT hrQueryInfoQueue = device->QueryInterface(IID_PPV_ARGS(&infoQueue));
	if (SUCCEEDED(hrQueryInfoQueue))
	{
		infoQueue->RegisterMessageCallback(ValidationLayersDebugCallback,
			D3D12_MESSAGE_CALLBACK_IGNORE_FILTERS, nullptr, &VLayerCallbackCookie);

		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
	}
	else
	{
		DEBUG_LOG("Device query info queue to enable validation layers failed", LOGTYPE_WARNING)
	}

	DEBUG_LOG("Device query info queue to enable validation layers success", LOGTYPE_LOG)
#endif

	{
		const D3D12_COMMAND_QUEUE_DESC desc{
			.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
		};

		/**
		* DX12 can create queue 'on the fly' after device creation.
		* No need to specify in advance how many queues will be used by the device object.
		*/
		const HRESULT hrGFXCmdQueueCreated = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&graphicsQueue));
		if (FAILED(hrGFXCmdQueueCreated))
		{
			DEBUG_LOG("Graphics queue creation failed!", LOGTYPE_ERROR);
			return EXIT_FAILURE;
		}
		else
		{
			const LPCWSTR name = L"GraphicsQueue";
			graphicsQueue->SetName(name);

			DEBUG_LOG("Graphics queue creation succeded!", LOGTYPE_VALIDATION);
		}
	}

	{
		deviceFenceEvent = CreateEvent(nullptr, false, false, nullptr);
		if (!deviceFenceEvent)
		{
			DEBUG_LOG("Device fence event creation failed!", LOGTYPE_ERROR);
			return EXIT_FAILURE;
		}
		else
		{
			DEBUG_LOG("Device fence event creation succeded!", LOGTYPE_VALIDATION);
		}

		const HRESULT hrDeviceFenceCreated = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&deviceFence));
		if (FAILED(hrDeviceFenceCreated))
		{
			DEBUG_LOG("Swapchain fence creation failed!", LOGTYPE_ERROR);
			return EXIT_FAILURE;
		}
		else
		{
			const LPCWSTR name = L"DeviceFence";
			deviceFence->SetName(name);

			DEBUG_LOG("Swapchain fence creation succeded!", LOGTYPE_VALIDATION);
		}
	}

	DEBUG_LOG("Device creation and synchronisation success!", LOGTYPE_VALIDATION);
	return EXIT_SUCCESS;
}

int Renderer::InitSwapchain()
{
	DEBUG_LOG("Initializing Swapchain...", LOGTYPE_LOG)

	int *sizeX = new int();
	int *sizeY = new int();
	context.GetWindowSize(sizeX, sizeY);

	const DXGI_SWAP_CHAIN_DESC1 desc{
		.Width = static_cast<uint32_t>(*sizeX),
		.Height = static_cast<uint32_t>(*sizeY),
		.Format = sceneColorFormat,
		.Stereo = false,
		.SampleDesc = {.Count = 1, .Quality = 0 },
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = bufferingCount,
		.Scaling = DXGI_SCALING_STRETCH,
		.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
		.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
		.Flags = 0,
	};

	delete sizeX; delete sizeY;

	MComPtr<IDXGISwapChain1> swapchain1;
	const HRESULT hrSwapChainCreated = factory->CreateSwapChainForHwnd(graphicsQueue.Get(), glfwGetWin32Window(context.window), &desc, nullptr, nullptr, &swapchain1);
	if (FAILED(hrSwapChainCreated))
	{
		DEBUG_LOG("Swapchain creation failed!", LOGTYPE_ERROR);
		return EXIT_FAILURE;
	}
	else
	{
		DEBUG_LOG("Swapchain creation success!", LOGTYPE_LOG);
	}

	const HRESULT hrSwapChainCast = swapchain1.As(&swapchain);
	if (FAILED(hrSwapChainCast))
	{
		DEBUG_LOG("Swapchain cast failed!", LOGTYPE_ERROR);
		return EXIT_FAILURE;
	}


	// Query back-buffers
	for (uint32_t i = 0; i < bufferingCount; ++i)
	{
		const HRESULT hrSwapChainGetBuffer = swapchain->GetBuffer(i, IID_PPV_ARGS(&swapchainImages[i]));
		if (FAILED(hrSwapChainGetBuffer))
		{
			DEBUG_LOG(std::format("Get Swapchain Buffer {} failed!", i), LOGTYPE_ERROR);
			return EXIT_FAILURE;
		}
		else
		{
			const std::wstring name = L"SwapchainBackBuffer [" + std::to_wstring(i) + L"]";
			swapchainImages[i]->SetName(name.data());

			DEBUG_LOG(std::format("Get Swapchain Buffer {} success!", i), LOGTYPE_LOG);
		}
	}

	// Synchronization
	{
		swapchainFenceEvent = CreateEvent(nullptr, false, false, nullptr);
		if (!swapchainFenceEvent)
		{
			DEBUG_LOG("Swapchain fence event creation failed!", LOGTYPE_ERROR);
			return EXIT_FAILURE;
		}

		const HRESULT hrSwapChainFenceCreated = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&swapchainFence));
		if (FAILED(hrSwapChainFenceCreated))
		{
			DEBUG_LOG("Swapchain fence creation failed!", LOGTYPE_ERROR);
			return EXIT_FAILURE;
		}
		else
		{
			const LPCWSTR name = L"SwapchainFence";
			swapchainFence->SetName(name);

			DEBUG_LOG("Swapchain fence creation success!", LOGTYPE_LOG);
		}
	}

	DEBUG_LOG("Swapchain initialized", LOGTYPE_VALIDATION);
	return EXIT_SUCCESS;
}

int Renderer::InitCommands()
{
	// Allocators
	for (uint32_t i = 0; i < bufferingCount; ++i)
	{
		auto& cmdAlloc = cmdAllocs[i];

		const HRESULT hrCmdAllocCreated = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAlloc));
		if (FAILED(hrCmdAllocCreated))
		{
			DEBUG_LOG(std::format("Create command allocator {} failed!", i), LOGTYPE_ERROR);
			return EXIT_FAILURE;
		}
		else
		{
			const std::wstring name = L"CommandAlloc [" + std::to_wstring(i) + L"]";
			cmdAlloc->SetName(name.c_str());
			DEBUG_LOG(std::format("Create command allocator {} success!", i), LOGTYPE_LOG);
		}
	}

	// List
	{
		const HRESULT hrCmdListCreated = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAllocs[0].Get(), nullptr, IID_PPV_ARGS(&cmdList));
		if (FAILED(hrCmdListCreated))
		{
			DEBUG_LOG("Create Command List failed!", LOGTYPE_ERROR);
			return EXIT_FAILURE;
		}
		else
		{
			const LPCWSTR name = L"CommandList";
			cmdList->SetName(name);

			DEBUG_LOG("Create Command List success!", LOGTYPE_LOG);
		}
	}

	// Command list must be closed because we will start the frame by Reset()
	cmdList->Close();

	return EXIT_SUCCESS;
}

int Renderer::InitSceneTextures()
{
	// Color RT View Heap
	{
		const D3D12_DESCRIPTOR_HEAP_DESC desc{
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			.NumDescriptors = bufferingCount,
			.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			.NodeMask = 0,
		};

		const HRESULT hrCreateHeap = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&sceneRTViewHeap));
		if (FAILED(hrCreateHeap))
		{
			DEBUG_LOG("Create color RenderTarget ViewHeap failed", LOGTYPE_ERROR);
			return EXIT_FAILURE;
		}
		else
		{
			const LPCWSTR name = L"SceneRTViewHeap";
			sceneRTViewHeap->SetName(name);

			DEBUG_LOG("Create color RenderTarget ViewHeap Success", LOGTYPE_LOG);
		}


		// Create RT Views (for each frame)
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = sceneRTViewHeap->GetCPUDescriptorHandleForHeapStart();
		const UINT rtvOffset = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		for (uint32_t i = 0; i < bufferingCount; ++i)
		{
			device->CreateRenderTargetView(swapchainImages[i].Get(), nullptr, rtvHandle);
			rtvHandle.ptr += rtvOffset;
		}
	}

	// Depth Scene Texture
	{
		int* sizeX = new int();
		int* sizeY = new int();
		context.GetWindowSize(sizeX, sizeY);

		const D3D12_RESOURCE_DESC desc{
			.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
			.Alignment = 0u,
			.Width = static_cast<uint32_t>(*sizeX),
			.Height = static_cast<uint32_t>(*sizeY),
			.DepthOrArraySize = 1,
			.MipLevels = 1,
			.Format = sceneDepthFormat,
			.SampleDesc = DXGI_SAMPLE_DESC{
				.Count = 1,
				.Quality = 0,
			},
			.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
			.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
		};

		delete sizeX; delete sizeY;

		const D3D12_HEAP_PROPERTIES heap{
			.Type = D3D12_HEAP_TYPE_DEFAULT,
			.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
			.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
			.CreationNodeMask = 1,
			.VisibleNodeMask = 1,
		};

		const HRESULT hrCreateDepthTexture = device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &sceneDepthClearValue, IID_PPV_ARGS(&sceneDepthTexture));
		if (FAILED(hrCreateDepthTexture))
		{
			DEBUG_LOG("Create Scene Depth Texture failed", LOGTYPE_ERROR);
			return EXIT_FAILURE;
		}
		else
		{
			const LPCWSTR name = L"SceneDepthTexture";
			sceneDepthTexture->SetName(name);
			DEBUG_LOG("Create Scene Depth Texture success", LOGTYPE_LOG);
		}
	}

	// Depth Scene RT View Heap
	{
		const D3D12_DESCRIPTOR_HEAP_DESC desc{
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
			.NumDescriptors = 1,
			.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			.NodeMask = 0,
		};

		const HRESULT hrCreateHeap = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&sceneDepthRTViewHeap));
		if (FAILED(hrCreateHeap))
		{
			DEBUG_LOG("Create Depth ViewHeap failed!", LOGTYPE_ERROR);
			return EXIT_FAILURE;
		}
		else
		{
			const LPCWSTR name = L"SceneDepthViewHeap";
			sceneDepthRTViewHeap->SetName(name);

			DEBUG_LOG("Create Depth ViewHeap success", LOGTYPE_LOG);
		}

		/**
		* Create Depth View to use sceneDepthTexture as a render target.
		*/
		device->CreateDepthStencilView(sceneDepthTexture.Get(), nullptr, sceneDepthRTViewHeap->GetCPUDescriptorHandleForHeapStart());
	}

	DEBUG_LOG("Scene textures initialisation success!", LOGTYPE_VALIDATION);
	return EXIT_SUCCESS;
}
#pragma endregion

void Renderer::Run()
{
	DEBUG_LOG("Start running...", LOGTYPE_INFO)
	while (!context.ShouldClose())
	{
		context.StartFrame();
	}
}

#pragma region Uninitialization
void Renderer::Destroy()
{
	DEBUG_LOG("Destroying...", LOGTYPE_INFO);

	DestroySceneTextures();
	DestroyCommands();
	DestroySwapchain();
	DestroyDevice();
	DestroyFactory();

	context.Destroy();

	DEBUG_LOG("Destroy success", LOGTYPE_VALIDATION)
}

void Renderer::DestroyFactory()
{
	DEBUG_LOG("Destroying Factory...", LOGTYPE_LOG)
	factory = nullptr;

#if _DEBUG
	// Report live objects
	MComPtr<IDXGIDebug1> dxgiDebug = nullptr;

	const HRESULT hrDebugInterface = DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug));
	if (SUCCEEDED(hrDebugInterface))
	{
		dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
	}
	else
	{
		DEBUG_LOG("Validation layer uninitialized failed.", LOGTYPE_ERROR)
}
#endif
}

void Renderer::DestroyDevice()
{
	// Synchronization
	{
		CloseHandle(deviceFenceEvent);
		DEBUG_LOG("Destroy Device Fence Event...", LOGTYPE_LOG);
		deviceFenceEvent = nullptr;

		DEBUG_LOG("Destroying Device Fence...", LOGTYPE_LOG);
		deviceFence = nullptr;
	}

	// Queue
	{
		// GFX
		{
			DEBUG_LOG("Destroying Graphics Queue...", LOGTYPE_LOG);
			graphicsQueue = nullptr;
		}
	}

#if SA_DEBUG
	// Validation Layers (device-level)
	if (VLayerCallbackCookie)
	{
		MComPtr<ID3D12InfoQueue1> infoQueue = nullptr;

		const HRESULT hrQueryInfoQueue = device->QueryInterface(IID_PPV_ARGS(&infoQueue));
		if (SUCCEEDED(hrQueryInfoQueue))
		{
			infoQueue->UnregisterMessageCallback(VLayerCallbackCookie);
			VLayerCallbackCookie = 0;
		}
	}
#endif

	DEBUG_LOG("Destroying Device...", LOGTYPE_LOG);
	device = nullptr;
}

void Renderer::DestroySwapchain()
{
	{
		CloseHandle(swapchainFenceEvent);
		DEBUG_LOG("Destroying Swapchain Fence Event...", LOGTYPE_LOG);
		swapchainFenceEvent = nullptr;

		DEBUG_LOG("Destroying Swapchain Fence...", LOGTYPE_LOG);
		swapchainFence = nullptr;
	}

	for (uint32_t i = 0; i < bufferingCount; ++i)
	{
		DEBUG_LOG(std::format("Destroying Swapchain image {}...", i), LOGTYPE_LOG);
		swapchainImages[i] = nullptr;
	}

	DEBUG_LOG("Destroying Swapchain...", LOGTYPE_LOG);
	swapchain = nullptr;
}

void Renderer::DestroyCommands()
{
	DEBUG_LOG("Destroying Command List...", LOGTYPE_LOG);
	cmdList = nullptr;

	for (uint32_t i = 0; i < bufferingCount; ++i)
	{
		DEBUG_LOG(std::format("Destroying Command Allocator {}...", i), LOGTYPE_LOG);
		cmdAllocs[i] = nullptr;
	}
}

void Renderer::DestroySceneTextures()
{
	DEBUG_LOG("Destroying Scene Color RT ViewHeap...", LOGTYPE_LOG);
	sceneRTViewHeap = nullptr;

	DEBUG_LOG("Destroying Scene Depth RT ViewHeap...", LOGTYPE_LOG);
	sceneDepthRTViewHeap = nullptr;

	DEBUG_LOG("Destroying Scene Depth Texture...", LOGTYPE_LOG);
	sceneDepthTexture = nullptr;
}
#pragma endregion
