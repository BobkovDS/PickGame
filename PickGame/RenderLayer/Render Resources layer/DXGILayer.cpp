#include "DXGILayer.h"
#include <dxgidebug.h>
#include "../d3dx12.h"

//Link necessary d3d12 libraries
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "D3DCompiler.lib")

DXGILayer::DXGILayer()
{
	m_isInitialisation_RTV_done = false;
	m_isInitialisation_Full_done = false;

	m_rtFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
}

DXGILayer::~DXGILayer()
{
	m_cmdFence.flush();
}

void DXGILayer::init(HWND hwnd, u16 width, u16 height)
{
	HRESULT res = 0;
	UINT dxgiFactoryFlags = 0;

	// If we in debug mode, turn on Debug mode DXGI and will create a DXGIFactory with debug Flag too

#if defined(_DEBUG1)
	{
		winrt::com_ptr<ID3D12Debug1> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.put()))))
		{
			debugController->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif

	// Create a Factory
	{
		res = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(m_factory.put()));
		
		assert(SUCCEEDED(res));
	}

	// Get required Adapter and create a Device
	{
		// Get required Adapter
		winrt::com_ptr<IDXGIAdapter1> hardwareAdapter;
		getHardwareAdapter(m_factory.get(), hardwareAdapter.put());

		// Create Device
		res = D3D12CreateDevice(hardwareAdapter.get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(m_device.put()));
		assert(SUCCEEDED(res));
	}

	// Create Command Queue
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		res = m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_cmdQueue.put()));
		assert(SUCCEEDED(res));
	}

	// Create Graphic Command List (for all other not graphical work)
	{
		// Command list allocator
		res = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_cmdAllocator.put()));
		assert(SUCCEEDED(res));

		// Command list itself
		res = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_cmdAllocator.get(),
			nullptr, IID_PPV_ARGS(m_cmdList.put()));
		assert(SUCCEEDED(res));
		m_cmdList->Close();
	}

	// Create Command Fence object
	m_cmdFence.create(m_device.get(), m_cmdQueue.get(), 1, L"DXGILayer");

	// Create SwapChain
	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = width;
		swapChainDesc.Height = height;
		swapChainDesc.BufferCount = sSwapChainBufferCount;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.Format = m_rtFormat;
		swapChainDesc.SampleDesc.Count = 1;

		winrt::com_ptr<IDXGISwapChain1> swapChain1;
		res = m_factory->CreateSwapChainForHwnd(m_cmdQueue.get(), hwnd, &swapChainDesc,
			nullptr, nullptr, swapChain1.put());
		assert(SUCCEEDED(res));
		swapChain1.as(m_swapChain);

		// Disable the Alt+Enter fullscreen toggle feature
		res = m_factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
		assert(SUCCEEDED(res));

		// Create Descriptor heap for RenderTargets Views
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.NumDescriptors = sSwapChainBufferCount;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		res = m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(m_rtvHeap.put()));
		assert(SUCCEEDED(res));
		m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		create_RTV(width, height);
		m_isInitialisation_RTV_done = true;
	}

	m_isInitialisation_Full_done = true;
}

void DXGILayer::resize(u16 width, u16 height)
{
	if (m_isInitialisation_RTV_done)
		create_RTV(width, height);
}

void DXGILayer::provideDXGIInformation(DXGILayerInformation& dxgiData)
{
	assert(m_isInitialisation_Full_done);

	dxgiData.pDevice = m_device.get();
	dxgiData.pSwapChain = m_swapChain.get();
	dxgiData.pSwapChainBuffers = m_swapChainBuffers;
	dxgiData.pCmdList = m_cmdList.get();
	dxgiData.pCmdAllocator = m_cmdAllocator.get();
	dxgiData.pRTVHeap = m_rtvHeap.get();
	dxgiData.pCmdQueue = m_cmdQueue.get();
	dxgiData.BackBufferFormat = m_rtFormat;
}

void DXGILayer::resetCommandList()
{
	auto res = m_cmdList->Reset(m_cmdAllocator.get(), nullptr);
	assert(SUCCEEDED(res));
}

void DXGILayer::executeCommandList()
{
	m_cmdList->Close();
	ID3D12CommandList* cmdList[] = { m_cmdList.get() };
	m_cmdQueue->ExecuteCommandLists(1, cmdList);

	flushCommandQueue();
}

void DXGILayer::flushCommandQueue()
{
	m_cmdFence.flush();
}

// ----------------------------------- Private methods -----------------------------------------------

void DXGILayer::getHardwareAdapter(IDXGIFactory4 * pFactory, IDXGIAdapter1 ** ppAdapter)
{
	if (pFactory == nullptr) return;

	
	for (u8 adapterId = 0; pFactory->EnumAdapters1(adapterId, ppAdapter) != DXGI_ERROR_NOT_FOUND; adapterId++)
	{
		DXGI_ADAPTER_DESC1 desc;
		(*ppAdapter)->GetDesc1(&desc);
		
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			// do not select the Basic Render Device Adapter
			continue;
		}

		// Check to see if the adapter support DirectX 12, but do not create the actual device yet
		if (SUCCEEDED(D3D12CreateDevice(*ppAdapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
			break;
	}
}

void DXGILayer::create_RTV(u16 width, u16 height)
{
	assert(m_device.get());
	assert(m_swapChain.get());

	// Wait when all commands on GPU are done //TODO: is it ok to wait in this way
	m_cmdFence.flush();
	
	HRESULT res = 0;

	// Reset SwapChain buffer
	for (u8 i = 0; i < sSwapChainBufferCount; i++)
	{
		m_swapChainBuffers[i] = nullptr;
	}

	res = m_swapChain->ResizeBuffers(sSwapChainBufferCount, width, height, m_rtFormat, 0);
	assert(SUCCEEDED(res));

	// Extract ID3DResource for SwapChain buffers
	for (u8 i = 0; i < sSwapChainBufferCount; i++)
	{
		m_swapChain->GetBuffer(i, IID_PPV_ARGS(m_swapChainBuffers[i].put()));

		std::wstring name(L"SwapChain_");
		name += std::to_wstring(i);
		m_swapChainBuffers[i]->SetName(name.c_str());
	}

	// Create RenderTarget Views for SwapChain buffers resources
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (u8 i = 0; i < sSwapChainBufferCount; i++)
	{
		m_device->CreateRenderTargetView(m_swapChainBuffers[i].get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, m_rtvDescriptorSize);
	}
}