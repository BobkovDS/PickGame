#pragma once
#include "SceneBoardDataBuilder.h"
#include "SceneEnvironment.h"
#include "SceneObjectLayerIterator.h"

class ResourceManager;

class SceneDataBuilder
{
	SceneEnvironmentConnector* m_sceneEnvConnector;
	SceneBoardDataBuilder m_skinnedBoardData;
	SceneObjectLayerIterator m_iterator;

	ResourceManager* m_resourceManager;

	void updateGPUData_();
	void updateSRVData_();
public:
	SceneDataBuilder();
	~SceneDataBuilder();

	void setConnectorSkinData(SkinDataConnector* connector);
	void setConnectorSceneEnvironment(SceneEnvironmentConnector* connector);
	void setConnectorBoardData(const GameBoardCurrentStatus* boardData);
	void setResourceManager(ResourceManager* resourceMngr);

	void update();
	SceneObjectLayerIterator* getIterator();
};

