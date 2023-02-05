#pragma once
#include "..\GameBoardLayer\GameBoard.h"
#include "..\GameBoardLayer\GameRulesPolicyManager.h"
#include "..\GameBoardLayer\GameRoundTaskCreater.h"
#include "..\RenderLayer\Render Resources layer\SkinManager.h"
#include "..\RenderLayer\Render Resources layer\SceneEnvironmentManager.h"
#include "..\RenderLayer\Render Resources layer\ResourceManager.h"
#include "..\RenderLayer\Render Resources layer\SceneDataBuilder.h"
#include "..\RenderLayer\Render Resources layer\GPUCanvas.h"
#include "..\RenderLayer\Render Resources layer\PSOManager.h"
#include "..\RenderLayer\Render\RenderManager.h"
#include "IOManager.h"

class Camera;

class GameManager
{
	GPUCanvas m_gpuCanvas;
	
	GameBoard m_gameBoard;
	GameRoundTaskCreater m_taskCreater;
	SceneDataBuilder m_sceneDataBuilder;

	GameRulesPolicyManager m_rulesPoliciesMngr;
	ResourceManager m_resourceManager;
	SkinManager m_skinManager;
	PSOManager m_psoManager;
	SceneEnvironmentManager m_sceneEnvironmentManager;
	RenderManager m_renderManager;
	IOManager m_ioManager;
	Camera* m_camera;

	void buildCamera();
	void update();

public:
	GameManager();
	~GameManager();

	void build(HINSTANCE hInstance);
	void resize(int width, int height);
};