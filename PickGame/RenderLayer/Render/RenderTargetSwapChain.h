#pragma once
#include "RenderResource.h"
#include "../Render Resources layer/DXGILayer.h"

/*
	Provide support of MSAA for SwapChain
*/

struct RenderTargerSwapChainParams
{
	ID3D12Device* pDevice;
	IDXGISwapChain3* pdxgiSwapChain;
	ID3D12GraphicsCommandList* pCmdList;
	ID3D12DescriptorHeap* prtvHeap;
	winrt::com_ptr<ID3D12Resource>* pSwapChainBuffers;
	DXGI_FORMAT rtFormat;
	u16 width;
	u16 height;
	D3D12_CLEAR_VALUE pClearValue ;
};

class RenderTargetSwapChain
{
	u16 tmpCount = 3;

	RenderResource m_renderTargets[DXGILayer::sSwapChainBufferCount];
	bool m_isMSAAUsed;
	bool m_msaaQualityWasChecked;
	bool m_initialized;
	u8 m_renderTargetsCount;
	u8 m_currentRenderTargetId;
	u32 m_rtDescriptorSize;
	D3D12_VIEWPORT m_viewPort;
	D3D12_RECT m_scissorRect;

	ID3D12Device* m_device;
	IDXGISwapChain3* m_pdxgiSwapChain;
	winrt::com_ptr<ID3D12Resource>* m_pSwapChainBuffers;
	ID3D12GraphicsCommandList* m_cmdList;
	DXGI_FORMAT m_resourceFormat;
	D3D12_CLEAR_VALUE m_optClear;
	DXGI_SAMPLE_DESC m_sampleDesc;
	DXGI_SAMPLE_DESC m_checkedSampleDesc;
	winrt::com_ptr<ID3D12DescriptorHeap> m_rtvHeap;
	ID3D12DescriptorHeap* m_rtvHeapPtr; // m_isMSAAUsed ? Points to our own RTV Heap : points to DXGI SwapChain RTV Heap
	ID3D12DescriptorHeap* m_rtvHeapPtrDXGI; // RTV Heap which comes from DXGI SwapChain

	void defineSampleDescription();
	COPY_FORBID(RenderTargetSwapChain)
public:
	RenderTargetSwapChain();
	~RenderTargetSwapChain();

	void initialize(RenderTargerSwapChainParams& params);

	void createDescriptorHeap_RTV();
	void createRTV();
	const D3D12_CPU_DESCRIPTOR_HANDLE getCurrentRTVHeapCPUHandle();
	ID3D12DescriptorHeap* getRTVHeapPtr();
	void setMSAASupport(bool value);
	void resize(u16 width, u16 height);
	void prepareToDraw();
	void setRenderTargetInfo();
	void prepareToPresent();
	void present();

	const D3D12_CLEAR_VALUE& getClearValue();
	DXGI_SAMPLE_DESC getSampleDesc();
};

inline ID3D12DescriptorHeap* RenderTargetSwapChain::getRTVHeapPtr()
{
	return m_rtvHeapPtr;
}

inline const D3D12_CLEAR_VALUE& RenderTargetSwapChain::getClearValue()
{
	if (m_isMSAAUsed)
		return m_renderTargets[m_currentRenderTargetId].getClearValue();
	else
		return m_optClear;
}

inline DXGI_SAMPLE_DESC RenderTargetSwapChain::getSampleDesc()
{
	defineSampleDescription();
	return m_sampleDesc;
}