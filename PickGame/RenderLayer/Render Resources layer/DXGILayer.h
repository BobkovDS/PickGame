#pragma once
#define _VSDESIGNER_DONT_LOAD_AS_DLL

#include <d3d12.h>
#include <dxgi1_4.h>
#include "..\..\Include\datatypedef.h"
#include <winrt/base.h>
#include "CommandFence.h"

struct DXGILayerInformation
{
	ID3D12Device* pDevice;
	IDXGISwapChain3* pSwapChain;
	ID3D12GraphicsCommandList* pCmdList;
	ID3D12CommandAllocator* pCmdAllocator;
	ID3D12DescriptorHeap* pRTVHeap;
	ID3D12CommandQueue* pCmdQueue;
	winrt::com_ptr<ID3D12Resource>* pSwapChainBuffers;
	DXGI_FORMAT BackBufferFormat;
};

class DXGILayer
{
public:
	const static u8 sSwapChainBufferCount = 3;

private:
	bool m_isInitialisation_RTV_done;
	bool m_isInitialisation_Full_done;
	DXGI_FORMAT m_rtFormat;
	u32 m_rtvDescriptorSize;
	CommandFence m_cmdFence;

	winrt::com_ptr<IDXGIFactory4> m_factory;
	IDXGIFactory4* m_factory2;
	winrt::com_ptr<IDXGISwapChain3> m_swapChain;
	winrt::com_ptr<ID3D12Device> m_device;
	winrt::com_ptr<ID3D12CommandQueue> m_cmdQueue;
	winrt::com_ptr<ID3D12GraphicsCommandList> m_cmdList;
	winrt::com_ptr<ID3D12CommandAllocator> m_cmdAllocator;
	winrt::com_ptr<ID3D12DescriptorHeap> m_rtvHeap;
	winrt::com_ptr<ID3D12Resource> m_swapChainBuffers[sSwapChainBufferCount];

	void create_RTV(u16 width, u16 height);

	void getHardwareAdapter(IDXGIFactory4* pFactory, IDXGIAdapter1** ppAdapter);
	COPY_FORBID(DXGILayer)
public:
	DXGILayer();
	~DXGILayer();
	void init(HWND hwnd, u16 width, u16 height);
	void resize(u16 width, u16 height);
	void provideDXGIInformation(DXGILayerInformation& dxgiData);
	void resetCommandList();
	void executeCommandList();
	void flushCommandQueue();
};