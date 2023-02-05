#include "SceneEnvironmentLoader.h"
#include "PGFFileLoader.h"

SceneEnvironmentLoader::SceneEnvironmentLoader():
	m_loader(nullptr)
{
	m_loader = new PGFFileLoader();
}

SceneEnvironmentLoader::~SceneEnvironmentLoader()
{
	if (m_loader)
		delete m_loader;
}

void SceneEnvironmentLoader::loadSceneEnvironmentFiles(std::vector<SceneEnvironment*>& sceneEnvironments,
	ResourceManager* resourceManager)
{
	if (m_loader)
	{
		SceneEnvironment* newSceneEnvironment = new SceneEnvironment(); // will be deleted by SceneEnvironmentManager

		m_loader->loadFile("default_sceneenv.fbx");
		sceneEnvironments.push_back(newSceneEnvironment);
	}
}