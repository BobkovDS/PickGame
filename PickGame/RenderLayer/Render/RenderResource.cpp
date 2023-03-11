#include "RenderResource.h"

ID3D12Device* RenderResource::m_spDevice = nullptr;

RenderResource::RenderResource()
{	
	m_description = {};
}

RenderResource::~RenderResource()
{
}

void RenderResource::create(
	DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flags, u16 width, u16 height,
	DXGI_SAMPLE_DESC* sampleDesc, D3D12_CLEAR_VALUE* optClear,
	D3D12_RESOURCE_STATES initialResourceState,
	u16 texturesCount)
{
	assert(m_spDevice != nullptr);

	DXGI_SAMPLE_DESC defSample= { 1,0 };

	HRESULT res = 0;
	
	m_initialResourceState = initialResourceState;
	m_description.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	m_description.Alignment = 0;
	m_description.Width = width;
	m_description.Height = height;
	m_description.DepthOrArraySize = texturesCount;
	m_description.MipLevels = 1;
	m_description.Format = format;
	m_description.SampleDesc = sampleDesc ? *sampleDesc : defSample;
	m_description.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	m_description.Flags = flags;

	if (optClear)
		m_optClear = *optClear;

	m_viewPort = {};
	m_viewPort.Width = static_cast<float> (width);
	m_viewPort.Height = static_cast<float> (height);
	m_viewPort.MinDepth = 0.0f;
	m_viewPort.MaxDepth = 1.0f;

	m_scissorRect = { 0,0, static_cast<LONG> (width),static_cast<LONG> (height) };

	res = m_spDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&m_description,
		m_initialResourceState,
		&m_optClear,
		IID_PPV_ARGS(m_gpu_resource.put()));

	assert(SUCCEEDED(res));
}

void RenderResource::resize(u16 width, u16 height)
{
	assert(m_spDevice != nullptr);

	m_gpu_resource = nullptr; // TODO: Is it enough to release a memory?
	
	create(m_description.Format, m_description.Flags, width, height, &m_description.SampleDesc, &m_optClear,
		m_initialResourceState, m_description.DepthOrArraySize);
}