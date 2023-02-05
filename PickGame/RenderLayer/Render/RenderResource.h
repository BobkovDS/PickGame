#pragma once
#include <d3d12.h>
#include "../d3dx12.h"
#include "../../Include/datatypedef.h"
#include <winrt/base.h>

class RenderResource
{
	static ID3D12Device* m_spDevice;
	D3D12_RESOURCE_DESC m_description;
	D3D12_RESOURCE_STATES m_initialResourceState;
	D3D12_CLEAR_VALUE m_optClear;
	D3D12_VIEWPORT m_viewPort;
	D3D12_RECT m_scissorRect;
	winrt::com_ptr<ID3D12Resource> m_gpu_resource;
	u16 m_texturesCount;

	COPY_FORBID(RenderResource)
public:
	RenderResource();
	~RenderResource();
	
	void create(DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flags, u16 width, u16 height,
		DXGI_SAMPLE_DESC* sampleDesc = nullptr, D3D12_CLEAR_VALUE* optClear = nullptr,
		D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_COMMON, u16 texturesCount = 1);
	
	void resize(u16 width, u16 height);
	
	void changeState(ID3D12GraphicsCommandList* cmdList,
		D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter);
	
	const D3D12_RESOURCE_BARRIER getBarrier(D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter);
	ID3D12Resource* getGPUResource();
	D3D12_VIEWPORT* getViewPort();
	D3D12_RECT* getScissorRect();
	const D3D12_CLEAR_VALUE& getClearValue();

	static void setDevice(ID3D12Device* device);
};


inline const D3D12_RESOURCE_BARRIER RenderResource::getBarrier(
	D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter)
{
	return CD3DX12_RESOURCE_BARRIER::Transition(m_gpu_resource.get(), stateBefore, stateAfter);
}

inline void RenderResource::changeState(ID3D12GraphicsCommandList* cmdList, D3D12_RESOURCE_STATES stateBefore,
	D3D12_RESOURCE_STATES stateAfter)
{
	cmdList->ResourceBarrier(0, &getBarrier(stateBefore, stateAfter));
}

inline ID3D12Resource* RenderResource::getGPUResource()
{
	return m_gpu_resource.get();
}

inline D3D12_VIEWPORT* RenderResource::getViewPort()
{
	return &m_viewPort;
}

inline D3D12_RECT* RenderResource::getScissorRect()
{
	return &m_scissorRect;
}

inline void RenderResource::setDevice(ID3D12Device* device)
{
	m_spDevice = device;
}

inline const D3D12_CLEAR_VALUE& RenderResource::getClearValue()
{
	return m_optClear;
}