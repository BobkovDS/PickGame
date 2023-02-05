#pragma once
#include "SceneEnvironmentLoader.h"
class SceneEnvironmentManager
{
	SceneEnvironmentConnector m_connector;
	std::vector<SceneEnvironment*> m_sceneEnvironments;
	SceneEnvironmentLoader m_loader;
public:
	SceneEnvironmentManager();
	~SceneEnvironmentManager();

	SceneEnvironmentConnector* getConnector();
	void loadSceneEnvironments(ResourceManager* resourceManager);
	void setSceneEnvironment(u8 env_id);
};

