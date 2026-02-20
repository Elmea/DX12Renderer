#include <DXGIDebug.h>

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



	DEBUG_LOG("Swapchain itinilisation not yet implemented", LOGTYPE_WARNING);
	return EXIT_SUCCESS;
}

#pragma endregion

void Renderer::Run()
{
	DEBUG_LOG("Start running", LOGTYPE_INFO)
	while (!context.ShouldClose())
	{
		context.StartFrame();
	}
}

void Renderer::Destroy()
{
	context.Destroy();
}