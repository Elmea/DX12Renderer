#include <d3d12.h> 

#include <DXGIDebug.h>

#include "Renderer.h"
#include "DebugMacros.h"

int Renderer::InitFactory()
{
	DEBUG_LOG("Initializing Factory", LOGTYPE_INFO)

	UINT dxgiFactoryFlags = 0;

// #ifdef _DEBUG
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
				DEBUG_LOG("Validation layer DebugController initialization failed.", LOGTYPE_ERROR)
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
				DEBUG_LOG("Validation layer DebugInfoQueue uninitialized failed.", LOGTYPE_ERROR);
			}
		}
	}

	// Enable additional debug layers.
	dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
// #endif

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

int Renderer::Init()
{
	int initializationResult = 0;
#define INITRES(func) if(!initializationResult) initializationResult = initializationResult || func;

	INITRES(context.Init(1200, 800, "DX12 Renderer"));
	INITRES(InitFactory());

	return initializationResult;
}

void Renderer::Run()
{
	DEBUG_LOG("Star running", LOGTYPE_INFO)
	while (!context.ShouldClose())
	{
		context.StartFrame();
	}
}

void Renderer::Destroy()
{
	context.Destroy();
}