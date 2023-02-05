#include "SceneEnvironmentManager.h"

SceneEnvironmentManager::SceneEnvironmentManager()
{
}

SceneEnvironmentManager::~SceneEnvironmentManager()
{
}

SceneEnvironmentConnector* SceneEnvironmentManager::getConnector()
{
	return &m_connector;
}

void SceneEnvironmentManager::loadSceneEnvironments(ResourceManager* resourceManager)
{
	m_loader.loadSceneEnvironmentFiles(m_sceneEnvironments, resourceManager);
	setSceneEnvironment(0);
}

void SceneEnvironmentManager::setSceneEnvironment(u8 env_id)
{
	if (m_sceneEnvironments.size() > 0)
		env_id %= m_sceneEnvironments.size();
	
	m_connector.SceneEnvData = m_sceneEnvironments[env_id];
	m_connector.IsNewData = true;
}