#include "GameManager.h"
#include "Executer.h"
#include "..\RenderLayer\Render Resources layer\Utilit3D.h"
#include "..\RenderLayer\Render Resources layer\Camera.h"

#include "pix3.h"

GameManager::GameManager():
	m_camera(nullptr)
{
}

GameManager::~GameManager()
{
}

void GameManager::build(HINSTANCE hInstance)
{
	m_rulesPoliciesMngr.loadPolicies("RulesPolicies");
	m_taskCreater.buildGameRoundTask(m_rulesPoliciesMngr.getPolicy(1));
	m_gameBoard.buildBoard(*m_taskCreater.getGameRoundTask());

	const GameBoardCurrentStatus* boardData = m_gameBoard.getCurrentGameData();
	m_gameBoard.makeTurn();

	// Create a Window and initialize DXGI layer
	m_gpuCanvas.init(hInstance);

	// Set 'work' function
	m_gpuCanvas.setWorkFunction(new ExecuterVoidVoid<GameManager>(this, &GameManager::update));

	// Set resize function
	m_gpuCanvas.setResizeFunction(new ExecuterVoidIntInt<GameManager>(this, &GameManager::resize));

	DXGILayerInformation dxgiData = {};
	m_gpuCanvas.provideDXGIData(dxgiData);
	
	Utilit3D::initialize(dxgiData.pDevice, dxgiData.pCmdList);

	//Build PSO Objects
	m_psoManager.buildPSOList(&dxgiData);

	// Load resources (models, textures etc)
	m_resourceManager.initialize(m_gpuCanvas.dxgiLayer());

	m_skinManager.loadSkins(&m_resourceManager); // TODO: Add SetResourceManager() method
	m_sceneEnvironmentManager.loadSceneEnvironments(&m_resourceManager); // TODO: Add SetResourceManager() method
	m_sceneDataBuilder.setConnectorSkinData(m_skinManager.getConnector());
	m_sceneDataBuilder.setConnectorSceneEnvironment(m_sceneEnvironmentManager.getConnector());
	m_sceneDataBuilder.setConnectorBoardData(m_gameBoard.getCurrentGameData());
	m_sceneDataBuilder.setResourceManager(&m_resourceManager);
	
	// Initialize Render layer
	RenderManagerParams renderMngrParams = {};
	renderMngrParams.DXGIData = dxgiData;
	renderMngrParams.pResourceManager = &m_resourceManager;
	renderMngrParams.pPSOManager = &m_psoManager;
	renderMngrParams.pSceneLayerIterator = m_sceneDataBuilder.getIterator();
	renderMngrParams.WindowSize = { m_gpuCanvas.getWidth(), m_gpuCanvas.getHeight() };
	m_renderManager.initialize(renderMngrParams);
	m_renderManager.buildRenders();

	m_gpuCanvas.dxgiLayer()->flushCommandQueue();

	buildCamera();

	m_ioManager.initialize(&m_gpuCanvas);
	m_ioManager.setCamera(m_camera);

	// Run Game Loop
	m_gpuCanvas.run(); // NO CODE AFTER THIS!!!
}

void GameManager::buildCamera()
{
	m_camera = new Camera();

	DirectX::XMVECTOR pos = DirectX::XMVectorSet(10.0f, 15.0f, 5.0f, 1.0f);
	DirectX::XMVECTOR target = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	m_camera->lookAt(pos, target, up);

	float w = static_cast<float> (m_gpuCanvas.getWidth());
	float h = static_cast<float> (m_gpuCanvas.getHeight());
	float aspect = w / h;
	m_camera->lens->setLens(0.25f * 3.1415f, aspect, 1.0f, 1000.0f);
}

void GameManager::update()
{
	using namespace DirectX;
	PIXScopedEvent(PIX_COLOR(155, 100, 0), "GameManager_Update");

	m_resourceManager.getFrameResourceManager()->getFreeFR();
	m_ioManager.update();

	auto passCBResource = m_resourceManager.getFrameResourceManager()->getMainPassCB();
	assert(passCBResource);

	XMMATRIX view = m_camera->getView();
	XMMATRIX proj = m_camera->lens->getProj();
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	PassConstantsGPU passCB = {};
	XMStoreFloat4x4(&passCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&passCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&passCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&passCB.ViewProj, XMMatrixTranspose(viewProj));

	passCB.EyePosW = m_camera->getPosition3f();

	// Light
	LightGPU& light = passCB.Lights[0];
	DirectX::XMVECTOR pos, direction;
	pos = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	direction = DirectX::XMVectorSet(0.0f, -0.2f, -0.7f, 0.0f);

	light.lightType = 1;
	light.Strength = XMFLOAT3(0.8f, 0.8f, 0.8f);
	DirectX::XMStoreFloat3(&light.Position, pos);
	DirectX::XMStoreFloat3(&light.Direction, direction);
	light.turnOn = 1;

	passCBResource->copyData(0, reinterpret_cast<BYTE*>(&passCB));

	m_sceneDataBuilder.update();
	m_renderManager.drawScene();
}

void GameManager::resize(int width, int height)
{
	m_renderManager.resize(width, height);
}