#include "RenderBase.h"
#include "RenderTargetSwapChain.h"
#include "..\Render Resources layer\ResourceManager.h"
#include "..\Render Resources layer\SceneObjectLayerIterator.h"
#include "..\Render Resources layer\PSOObject.h"

#include "../d3dx12.h"

RenderBase::RenderBase() :
	m_initialized(false),
	m_device(nullptr),
	m_cmdList(nullptr),
	m_resourceManager(nullptr),
	m_sceneLayerIterator(nullptr),
	m_renderTargetSwapChain(nullptr),
	m_PSOObject(nullptr),
	m_own_DSResource(nullptr),
	m_pDSVHeap(nullptr),
	m_pRTVHeap(nullptr),
	m_pSRV_UAVHeap(nullptr)
{

}

RenderBase::~RenderBase()
{
	if (m_own_DSResource) delete m_own_DSResource;

	for (auto rs : m_own_RTResources)
		delete rs;

	for (auto rs : m_own_SRResources)
		delete rs;

	for (auto rs : m_own_UAResources)
		delete rs;

	m_own_RTResources.clear();
	m_own_SRResources.clear();
	m_own_UAResources.clear();
}

void RenderBase::init(RenderBaseParameters & params)
{
	assert(params.pDevice);
	assert(params.pCmdList);
	assert(params.pRenderTargetSwapChain);

	m_device = params.pDevice;
	m_cmdList = params.pCmdList;
	m_resourceManager = params.pResourceManager;
	m_sceneLayerIterator = params.pSceneLayerIterator;
	m_PSOObject = params.pPSOObject;
	m_rtvDescritorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_renderTargetSwapChain = params.pRenderTargetSwapChain;

	m_initialized = true;
}

void RenderBase::resize(u16 width, u16 height) /*virtual*/
{
	// Change a size for DepthStencil Resource, if we have it
	if (m_own_DSResource)
	{
		m_own_DSResource->resize(width, height);
		createDSV();
	}

	// Change a size for RenderTarget resources, if we have it
	for (auto rtResource : m_own_RTResources)
		rtResource->resize(width, height);
	
	if (m_own_RTResources.size())
		createRTV();
}

void RenderBase::build() /*abstract virtual*/
{
}

void RenderBase::draw(u32 flags) /*abstract virtual*/
{
	draw_layers(255);
}

void RenderBase::addResourceDS(DXGI_FORMAT resourceFormat, u16 width, u16 height)
{
	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = resourceFormat;
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;

	m_own_DSResource = new RenderResource();
	m_own_DSResource->create(resourceFormat, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
		width, height, &m_renderTargetSwapChain->getSampleDesc(), &clearValue, D3D12_RESOURCE_STATE_DEPTH_WRITE , 1);
}

void RenderBase::addResourceSR(DXGI_FORMAT resourceFormat, u16 width, u16 height)
{
	m_own_SRResources.push_back(new RenderResource());
	m_own_SRResources[m_own_SRResources.size() - 1]->create(resourceFormat, D3D12_RESOURCE_FLAG_NONE,
		width, height);
}

void RenderBase::addResourceUA(DXGI_FORMAT resourceFormat, u16 width, u16 height)
{
	m_own_UAResources.push_back(new RenderResource());
	m_own_UAResources[m_own_UAResources.size() - 1]->create(resourceFormat, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		width, height);
}

void RenderBase::addResourceRT(DXGI_FORMAT resourceFormat, u16 width, u16 height, u8 resourcesCount)
{
	m_own_RTResources.push_back(new RenderResource());
	m_own_RTResources[m_own_RTResources.size() - 1]->create(resourceFormat, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
		width, height, nullptr, nullptr, D3D12_RESOURCE_STATE_RENDER_TARGET, resourcesCount);
}

void RenderBase::createDescriptorHeapDSV()
{
	// We have our own Depth-Stencil View Descriptor Heap
	assert(m_initialized);

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	desc.NumDescriptors = 1;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	
	assert(SUCCEEDED(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_own_dsvHeap.put()))));
	m_pDSVHeap = m_own_dsvHeap.get();
}

void RenderBase::createDescriptorHeapRTV(u8 descriptorsCount)
{
	// We have our own RenderTarget Views Descriptor Heap
	assert(m_initialized);

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	desc.NumDescriptors = descriptorsCount;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	assert(SUCCEEDED(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_own_rtvHeap.put()))));
	m_pRTVHeap = m_own_rtvHeap.get();
}

void RenderBase::setDescriptorHeapDSV(ID3D12DescriptorHeap * heap)
{
	// We do not have our own DSV Heap, we use it from outside
	m_pDSVHeap = heap;
}

void RenderBase::setDescriptorHeapRTV(ID3D12DescriptorHeap * heap)
{
	// We do not have our own RTV Heap, we use it from outside
	m_pRTVHeap = heap;
}

void RenderBase::setDescriptorHeapSRV(ID3D12DescriptorHeap * heap)
{
	// We do not have our own SRV_UAV Heap, we use it from outside
	m_pSRV_UAVHeap = heap;
}

void RenderBase::createDSV(DXGI_FORMAT viewFormat)
{
	assert(m_initialized);
		
	m_device->CreateDepthStencilView(m_own_DSResource->getGPUResource(), nullptr, 
		m_pDSVHeap->GetCPUDescriptorHandleForHeapStart());
}

void RenderBase::createRTV(DXGI_FORMAT viewFormat)
{
	assert(m_initialized);
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle = 
		CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pRTVHeap->GetCPUDescriptorHandleForHeapStart());

	for (auto rs : m_own_RTResources)
	{
		m_device->CreateRenderTargetView(rs->getGPUResource(), nullptr, handle);
		handle.Offset(1, m_rtvDescritorSize);
	}
}

ID3D12Resource * RenderBase::getResourceDS()
{
	return m_own_DSResource->getGPUResource();
}

ID3D12DescriptorHeap * RenderBase::getDSVHeapPtr()
{
	return m_pDSVHeap;
}

ID3D12DescriptorHeap * RenderBase::getRtvHeapPtr()
{
	return m_pRTVHeap;
}


// --------------------- Private methods ------------

void RenderBase::draw_layers(u16 requiredLayers)
{
	/*
		Here we assume, that:
		-RootSignature
		-Root Arguments
		-ViewPort and ScissorRect
		-Descriptor heaps

		have been already set before (in draw(u32) method)
	*/

	m_sceneLayerIterator->defineRequiredLayers(requiredLayers);

	std::vector<SceneObjectLayer*> layers;
	u32 offset = 0;
	u8 layerID = 0;
	while (m_sceneLayerIterator->next(offset, layerID, layers))
	{
		// Set Pipline Object for current layer
		ID3D12PipelineState* pPSO = m_PSOObject->getPSO(layerID);
		m_cmdList->SetPipelineState(pPSO); /* We think that all layers have the same RootSignature,
		so this call will not reset rootSignature parametrs, but will changes some details of PSO */

		// We have the same kind of layers for several SceneObjectHolders
		for (auto& layerPerHolder : layers)
		{
			// Get all objects for current layer and draw it using their LOD
			auto& sceneObjects = layerPerHolder->getSceneObjects();

			for (auto& sceneObject : sceneObjects)
			{
				
				u8 meshesCount = 0;
				auto sceneObjectDrawArgs = sceneObject->getDrawArgs(meshesCount);

				for (u8 i = 0; i < meshesCount; i++)
				{
					auto meshDrawArgs = sceneObjectDrawArgs[i].first->SubMesh;

					m_cmdList->SetGraphicsRoot32BitConstant(0, offset, 0);
					m_cmdList->IASetVertexBuffers(0, 1, sceneObjectDrawArgs[i].first->getVertexGPUBufferView());
					m_cmdList->IASetIndexBuffer(sceneObjectDrawArgs[i].first->getIndexGPUBuffer());
					m_cmdList->IASetPrimitiveTopology(sceneObjectDrawArgs[i].first->PrimitiveType);
					m_cmdList->DrawIndexedInstanced(meshDrawArgs.IndexCount, sceneObjectDrawArgs[i].second,
						meshDrawArgs.StartIndexCount, 0, 0);
					offset += sceneObjectDrawArgs[i].second;
				}
			}
		}
	}
}