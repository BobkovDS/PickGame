#pragma once
#include "RenderTargetSwapChain.h"

class ResourceManager;
class PSOManager;
class SceneObjectLayerIterator;
class FinalRender;

struct RenderManagerParams {
	DXGILayerInformation DXGIData;
	ResourceManager* pResourceManager;
	PSOManager* pPSOManager;
	SceneObjectLayerIterator* pSceneLayerIterator;
	SIZE WindowSize;
};

class RenderManager
{
	ID3D12Device* m_device;
	ID3D12CommandQueue* m_pCommandQueue;
	winrt::com_ptr<ID3D12GraphicsCommandList> m_cmdList;
	ResourceManager* m_resourceManager;
	SceneObjectLayerIterator* m_sceneLayerIterator;
	PSOManager* m_pPSOManager;
	RenderTargetSwapChain m_renderTargetSwapChain;
	D3D12_CLEAR_VALUE m_rtClearValue;
	SIZE m_windowSize;
	bool m_initialized;

	// -- Renders
	FinalRender* m_finalRender;
	// -- 
	void buildCmdList();
	COPY_FORBID(RenderManager)

public:
	RenderManager();
	~RenderManager();
	void initialize(RenderManagerParams& params);
	void buildRenders();
	void setDisplayOptions();
	void resize(u16 width, u16 height);
	void drawScene(u32 flags=0);
};