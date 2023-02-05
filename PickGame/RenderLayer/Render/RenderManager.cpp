#include "RenderManager.h"

#include "RenderBase.h"
#include "FinalRender.h"
#include "../Render Resources layer/PSOManager.h"
#include "../Render Resources layer/DXGILayer.h"
#include "../Render Resources layer/ResourceManager.h"

#include "pix3.h"

RenderManager::RenderManager():
	m_initialized(false),
	m_resourceManager(nullptr),
	m_sceneLayerIterator(nullptr),
	m_pPSOManager(nullptr),
	m_finalRender(nullptr)
{
	m_finalRender = new FinalRender();
}

RenderManager::~RenderManager()
{
	if (m_finalRender)
		delete m_finalRender;
}

void RenderManager::initialize(RenderManagerParams & params)
{
	assert(params.pResourceManager);
	assert(params.pSceneLayerIterator);
	assert(params.pPSOManager);
	assert(params.DXGIData.pDevice);
	assert(params.DXGIData.pCmdQueue);
	assert(params.DXGIData.pRTVHeap);
	assert(params.DXGIData.pSwapChain);
	assert(params.DXGIData.pSwapChainBuffers);

	m_device = params.DXGIData.pDevice;
	m_pCommandQueue = params.DXGIData.pCmdQueue;

	buildCmdList();

	m_resourceManager = params.pResourceManager;
	m_sceneLayerIterator = params.pSceneLayerIterator;
	m_pPSOManager = params.pPSOManager;

	RenderTargerSwapChainParams rtscParams = {};
	rtscParams.pDevice = params.DXGIData.pDevice;
	rtscParams.pdxgiSwapChain = params.DXGIData.pSwapChain;
	rtscParams.pSwapChainBuffers = params.DXGIData.pSwapChainBuffers;
	rtscParams.pCmdList = m_cmdList.get();
	rtscParams.prtvHeap = params.DXGIData.pRTVHeap;
	rtscParams.rtFormat = params.DXGIData.BackBufferFormat;
	rtscParams.width = (u16)params.WindowSize.cx;
	rtscParams.height = (u16)params.WindowSize.cy;

	float rtClearColor[4] = { 0.0f, 0.5f, 0.4f, 1.0f };
	//rtscParams.pClearValue = {};
	rtscParams.pClearValue.Format = rtscParams.rtFormat;
	std::copy(rtClearColor, rtClearColor + 4, rtscParams.pClearValue.Color);

	m_renderTargetSwapChain.initialize(rtscParams);

	m_windowSize = params.WindowSize;
	m_initialized = true;

	m_cmdList->Close();
}

void RenderManager::buildRenders()
{
	// Prepare RenderParams
	RenderBaseParameters renderParams = {};
	renderParams.pDevice = m_device;
	renderParams.pCmdList = m_cmdList.get();
	renderParams.pResourceManager = m_resourceManager;
	renderParams.pSceneLayerIterator = m_sceneLayerIterator;
	renderParams.pRenderTargetSwapChain = &m_renderTargetSwapChain;

	//

	renderParams.pPSOObject = m_pPSOManager->getPSOObject(PSOOBJECT_NAME_FINAL_RENDER);
	
	m_finalRender->init(renderParams);
	//m_finalRender->build();
	m_finalRender->addResourceDS(DXGI_FORMAT_D24_UNORM_S8_UINT, (u16) m_windowSize.cx, (u16) m_windowSize.cy);
	m_finalRender->createDescriptorHeapDSV();
	m_finalRender->createDSV();
	//m_finalRender->setDescriptorHeapSRV(m_resourceManager->getTextureHeap());
	//m_finalRender->setDescriptorHeapRTV(m_RenderTargetSwapChain)
}

void RenderManager::setDisplayOptions()
{
}

void RenderManager::resize(u16 width, u16 height)
{
	m_renderTargetSwapChain.resize(width, height);
	m_finalRender->resize(width, height);
}

void RenderManager::drawScene(u32 flags)
{
	PIXScopedEvent(PIX_COLOR(0, 250, 0), "RenderManager_DrawScene");
	m_resourceManager->getFrameResourceManager()->changeCmdAllocator(m_cmdList.get(), nullptr);

	m_renderTargetSwapChain.prepareToDraw();

	m_finalRender->draw(NULL);

	m_renderTargetSwapChain.prepareToPresent();
	m_cmdList->Close();
	ID3D12CommandList* cmdList[] = { m_cmdList.get() };
	m_pCommandQueue->ExecuteCommandLists(1, cmdList);
	
	// Render GUI

	m_renderTargetSwapChain.present();

	m_resourceManager->getFrameResourceManager()->frameDone();
}

// --------------------- Private methods ------------

void RenderManager::buildCmdList()
{	
	// Will use CommandAllocator from FrameResource for each frame, so we do not need our own CommandAllocator here
	
	//TODO: Are we sure that we do not need our own CommandAllocator?

	HRESULT res = 0;
	winrt::com_ptr<ID3D12CommandAllocator> tmpCommandAllocator;
	res = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(tmpCommandAllocator.put()));
	
	res = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, tmpCommandAllocator.get(), nullptr,
		IID_PPV_ARGS(m_cmdList.put()));

	m_cmdList->SetName(L"CommandList (RenderManager)");

	tmpCommandAllocator->SetName(L"Tmp Cmd Allocator");
	///m_cmdList->Close();
	assert(SUCCEEDED(res));
}