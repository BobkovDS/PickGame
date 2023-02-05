#include "RenderTargetSwapChain.h"
#include "../d3dx12.h"

RenderTargetSwapChain::RenderTargetSwapChain() :
	m_isMSAAUsed(true),
	m_msaaQualityWasChecked(false),
	m_initialized(false),
	m_currentRenderTargetId(0),
	m_device(nullptr),
	m_pdxgiSwapChain(nullptr),
	m_cmdList(nullptr),
	m_pSwapChainBuffers(nullptr),
	m_rtvHeapPtr(nullptr),
	m_rtvHeapPtrDXGI(nullptr)
{
}

RenderTargetSwapChain::~RenderTargetSwapChain()
{
}

void RenderTargetSwapChain::initialize(RenderTargerSwapChainParams& params)
{
	assert(params.pDevice);
	assert(params.pdxgiSwapChain);
	assert(params.pCmdList);
	assert(params.prtvHeap);
	assert(params.pSwapChainBuffers);

	m_renderTargetsCount = DXGILayer::sSwapChainBufferCount;
	m_device = params.pDevice;
	m_pdxgiSwapChain = params.pdxgiSwapChain;
	m_pSwapChainBuffers = params.pSwapChainBuffers;
	m_cmdList = params.pCmdList;
	m_resourceFormat = params.rtFormat;
	m_rtvHeapPtrDXGI = params.prtvHeap;

	RenderResource::setDevice(params.pDevice);

	defineSampleDescription();

	m_rtDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// Create own RTV heap for internal Render Target resources
	createDescriptorHeap_RTV();

	// Create RenderTarget resources
	for (u8 i = 0; i < DXGILayer::sSwapChainBufferCount; i++)
	{		
		m_renderTargets[i].create(
			params.rtFormat,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
			params.width,
			params.height,
			&m_sampleDesc,
			&params.pClearValue,
			D3D12_RESOURCE_STATE_RENDER_TARGET);
	}

	// Resise it and more
	resize(params.width, params.height);

	// TurnOn MSAA (it also select which RTV Heap to use)
	setMSAASupport(true);

	m_initialized = true;
}

void RenderTargetSwapChain::createDescriptorHeap_RTV()
{
	assert(m_device);

	//if (m_rtvHeap.get() != nullptr)		return;

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.NumDescriptors = (UINT) sizeof(m_renderTargets) / sizeof(RenderResource);
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	
	assert(SUCCEEDED(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(m_rtvHeap.put()))));
}

void RenderTargetSwapChain::createRTV()
{
	assert(m_device);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
		
	for (u8 i=0; i< m_renderTargetsCount; i++)
	{ 
		m_device->CreateRenderTargetView(m_renderTargets[i].getGPUResource(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, m_rtDescriptorSize);
	}
}

const D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetSwapChain::getCurrentRTVHeapCPUHandle()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle =
		CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvHeapPtr->GetCPUDescriptorHandleForHeapStart());

	cpuHandle.Offset(m_currentRenderTargetId, m_rtDescriptorSize);
	
	return cpuHandle;
}

void RenderTargetSwapChain::setMSAASupport(bool value)
{
	m_isMSAAUsed = value;

	if (m_isMSAAUsed)
		m_rtvHeapPtr = m_rtvHeap.get();
	else
		m_rtvHeapPtr = m_rtvHeapPtrDXGI;
}

void RenderTargetSwapChain::resize(u16 width, u16 height)
{
	assert(m_device);

	// It is for if MSAA not used
	{
		m_viewPort = {};
		m_viewPort.Width = static_cast<float> (width);
		m_viewPort.Height = static_cast<float> (height);
		m_viewPort.MinDepth = 0.0f;
		m_viewPort.MaxDepth = 1.0f;

		m_scissorRect = { 0,0, static_cast<LONG> (width),static_cast<LONG> (height) };
	}

	// If MSAA used, RenderResources have all info
	for (u8 i = 0; i < m_renderTargetsCount; i++)
	{
		m_renderTargets[i].resize(width, height);

#if defined(_DEBUG)
		std::wstring resourceName = L"RenderTargetSwapChain_MSAA_Resource_" + std::to_wstring(i);
		m_renderTargets[i].getGPUResource()->SetName(resourceName.c_str());
#endif
	}

	createRTV();

	m_currentRenderTargetId = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
}

void RenderTargetSwapChain::prepareToDraw()
{
	assert(m_pdxgiSwapChain);
	assert(m_cmdList);
	assert(m_pSwapChainBuffers);

	// 1 - If we do not use MSAA, we will Render directly to DXGI Swapchain buffers
	if (!m_isMSAAUsed)
	{
		u8 currentSwapChainBufferID = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

		const D3D12_RESOURCE_BARRIER* barriers[] =
		{
			&CD3DX12_RESOURCE_BARRIER::Transition(
				m_pSwapChainBuffers[currentSwapChainBufferID].get(),
				D3D12_RESOURCE_STATE_PRESENT,
				D3D12_RESOURCE_STATE_RENDER_TARGET
			)
		};

		m_cmdList->ResourceBarrier(1, *barriers);
	}
	else
	{

	}
}

void RenderTargetSwapChain::setRenderTargetInfo()
{
	m_cmdList->RSSetViewports(
		1, m_isMSAAUsed ? m_renderTargets[m_currentRenderTargetId].getViewPort() : &m_viewPort);

	m_cmdList->RSSetScissorRects(
		1, m_isMSAAUsed ? m_renderTargets[m_currentRenderTargetId].getScissorRect() : &m_scissorRect);

	//m_cmdList->RSSetViewports( 1, &m_viewPort);

	//m_cmdList->RSSetScissorRects(1,&m_scissorRect);
}

void RenderTargetSwapChain::prepareToPresent()
{
	assert(m_pdxgiSwapChain);
	assert(m_cmdList);
	assert(m_pSwapChainBuffers);

	if (m_isMSAAUsed)
	{
		/*
			If use a MSAA, it means we do not draw to IDXGISwapChain.Buffers directly, and only copy 
			from RenderTargetSwapChain.buffer to IDXGISwapChain.Buffer. So we do:
			1) Change a status for IDXGISwapChain.Buffer from 'Present' to 'Resolve_Source'
			2) Copy from RenderTargeSwapChain.Buffer to IDXGISwapChain.Buffer
			3) Change a status for IDXGISwapChain.Buffer from 'Resolve_Source' to 'Present'
		*/

		// 1 
		//if (tmpCount > 0)
		//{
		//	u8 currentSwapChainBufferID = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
		//	const D3D12_RESOURCE_BARRIER barriers[] =
		//	{
		//		CD3DX12_RESOURCE_BARRIER::Transition(m_pSwapChainBuffers[currentSwapChainBufferID].get(),
		//			D3D12_RESOURCE_STATE_RENDER_TARGET,
		//			D3D12_RESOURCE_STATE_PRESENT
		//			)
		//	};

		//	m_cmdList->ResourceBarrier(1, barriers);
		//	tmpCount--;
		//}

		u8 currentSwapChainBufferID = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
		const D3D12_RESOURCE_BARRIER barriers[] =
		{
			m_renderTargets[m_currentRenderTargetId].getBarrier(
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_RESOLVE_SOURCE),
			
			CD3DX12_RESOURCE_BARRIER::Transition(m_pSwapChainBuffers[currentSwapChainBufferID].get(),
				D3D12_RESOURCE_STATE_PRESENT,
				D3D12_RESOURCE_STATE_RESOLVE_DEST)
		};

		m_cmdList->ResourceBarrier(2, barriers);

		//  2
		m_cmdList->ResolveSubresource(
			m_pSwapChainBuffers[currentSwapChainBufferID].get(), 0,
			m_renderTargets[m_currentRenderTargetId].getGPUResource(), 0, m_resourceFormat
		);

		// 3
		const D3D12_RESOURCE_BARRIER barriersBack[] =
		{
			m_renderTargets[m_currentRenderTargetId].getBarrier(
				D3D12_RESOURCE_STATE_RESOLVE_SOURCE,
				D3D12_RESOURCE_STATE_RENDER_TARGET),

			CD3DX12_RESOURCE_BARRIER::Transition(m_pSwapChainBuffers[currentSwapChainBufferID].get(),
				D3D12_RESOURCE_STATE_RESOLVE_DEST,
				D3D12_RESOURCE_STATE_PRESENT)
		};

		m_cmdList->ResourceBarrier(2, barriersBack);
	}
	else
	{
		/*
			If DO NOT use a MSAA, it means we draw to IDXGISwapChain.Buffers directly. So it has a status 
			'Render_Target'. Now we need to change it to 'Present'
			
		*/

		u8 currentSwapChainBufferID = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
		const D3D12_RESOURCE_BARRIER* barriers[] =
		{			
			&CD3DX12_RESOURCE_BARRIER::Transition(m_pSwapChainBuffers[currentSwapChainBufferID].get(),
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PRESENT)
		};

		m_cmdList->ResourceBarrier(1, *barriers);
	}
}

void RenderTargetSwapChain::present()
{
	m_pdxgiSwapChain->Present(0, 0);
	m_currentRenderTargetId = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
}

// -------------------------- Private methods --------------------------

void RenderTargetSwapChain::defineSampleDescription()
{
	// Check Multisample quality levels, if this was not done before

	if (!m_msaaQualityWasChecked)
	{
		u8 msaaCount = 4;
		u8 msaaQuality = 0;

		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS qualityLevels;
		qualityLevels.SampleCount = msaaCount;
		qualityLevels.NumQualityLevels = msaaQuality;
		qualityLevels.Format = m_resourceFormat;
		qualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;

		if (SUCCEEDED(m_device->CheckFeatureSupport(
			D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
			&qualityLevels,
			sizeof qualityLevels)))
		{
			m_checkedSampleDesc.Count = qualityLevels.SampleCount;
			m_checkedSampleDesc.Quality = (qualityLevels.NumQualityLevels > 0) ? qualityLevels.NumQualityLevels - 1 : 0;
		}
		else
		{
			m_checkedSampleDesc.Count = 1;
			m_checkedSampleDesc.Quality = 0;
		}

		m_msaaQualityWasChecked = true;
	}

	// define SampleDescription
	m_sampleDesc.Count = m_isMSAAUsed ? m_checkedSampleDesc.Count : 1;
	m_sampleDesc.Quality = m_isMSAAUsed ? m_checkedSampleDesc.Quality : 0;
}