#pragma once
#include <vector>
#include "..\Render Resources layer\gpuResources_materails_and_textures.h"

class ResourceManager;
class SceneObjectLayerIterator;
class PSOObject;
class PSOObject;
class RenderTargetSwapChain;
class RenderResource;

struct RenderBaseParameters
{
	ID3D12Device* pDevice;
	ID3D12GraphicsCommandList* pCmdList;
	ResourceManager* pResourceManager;
	SceneObjectLayerIterator* pSceneLayerIterator;
	RenderTargetSwapChain* pRenderTargetSwapChain;
	PSOObject* pPSOObject;
};

class RenderBase
{
protected:

	bool m_initialized;
	ID3D12Device* m_device;
	ID3D12GraphicsCommandList* m_cmdList;
	ResourceManager* m_resourceManager;
	SceneObjectLayerIterator* m_sceneLayerIterator;
	RenderTargetSwapChain* m_renderTargetSwapChain;
	PSOObject* m_PSOObject;
	u32 m_rtvDescritorSize;

	RenderResource* m_own_DSResource;
	std::vector<RenderResource*> m_own_RTResources; //really stores RenderResources
	std::vector<RenderResource*> m_own_SRResources; //really stores RenderResources
	std::vector<RenderResource*> m_own_UAResources; //really stores RenderResources

	winrt::com_ptr<ID3D12DescriptorHeap> m_own_dsvHeap;
	winrt::com_ptr<ID3D12DescriptorHeap> m_own_rtvHeap;
	winrt::com_ptr<ID3D12DescriptorHeap> m_own_srv_uavHeap;
	D3D12_GPU_DESCRIPTOR_HANDLE m_srvHeapHandle;

	// --- interface
	/*RenderResource* m_pDSResource; // Points to m_own_DSResource
	std::vector<RenderResource*> m_pRTResources; // Points to m_own_RTResources
	std::vector<RenderResource*> m_pSRResources; // Points to m_own_resources
	std::vector<RenderResource*> m_pUAResources; // Points to m_own_resources*/
	ID3D12DescriptorHeap* m_pDSVHeap; // Can point to m_own_dsvHeap or can be set by RenderManager
	ID3D12DescriptorHeap* m_pRTVHeap; // Can point to m_own_rtvHeap or can be set by RenderManager
	ID3D12DescriptorHeap* m_pSRV_UAVHeap; // Can point to m_own_srv_uavHeap or can be set by RenderManager
	void draw_layers(u16 visibleLayers);

public:
	RenderBase();
	virtual ~RenderBase();
	void init(RenderBaseParameters& params);
	virtual void resize(u16 width, u16 height);
	virtual void build() = 0; // Do any other operations to build Render (to add more resources, create SRVs and etc)
	virtual void draw(u32 flags = 0) = 0;

	// ---- for Resources which this class can Own
	void addResourceDS(DXGI_FORMAT resourceFormat, u16 width, u16 height);
	void addResourceSR(DXGI_FORMAT resourceFormat, u16 width, u16 height);
	void addResourceUA(DXGI_FORMAT resourceFormat, u16 width, u16 height);
	void addResourceRT(DXGI_FORMAT resourceFormat, u16 width, u16 height,
		u8 resourcesCount = 1);
	void createDescriptorHeapDSV();
	void createDescriptorHeapRTV(u8 descriptorsCount = 1);
	void createDSV(DXGI_FORMAT viewFormat = DXGI_FORMAT_UNKNOWN);
	void createRTV(DXGI_FORMAT viewFormat = DXGI_FORMAT_UNKNOWN);
	ID3D12Resource* getResourceDS();
	//---- 

	//---- this can be set by RenderManager
	void setDescriptorHeapDSV(ID3D12DescriptorHeap* heap);
	void setDescriptorHeapRTV(ID3D12DescriptorHeap* heap);
	void setDescriptorHeapSRV(ID3D12DescriptorHeap* heap);

	ID3D12DescriptorHeap* getDSVHeapPtr();
	ID3D12DescriptorHeap* getRtvHeapPtr();
};

