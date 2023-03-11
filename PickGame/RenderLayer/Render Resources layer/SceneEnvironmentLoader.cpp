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

void SceneEnvironmentLoader::loadSceneEnvironmentFiles(
	std::vector<std::unique_ptr<SceneEnvironment>> &sceneEnvironments,
	ResourceManager* resourceManager)
{
	/*TODO: Here should be automatic loading existing skin list, so some skin_list_builder
	*/

	std::unique_ptr <SceneEnvironment> newSceneEnvironment;
	newSceneEnvironment.reset(new SceneEnvironment());

	m_loader->loadFile("default_sceneenv.fbx");
		
	if (!m_loader->IsSuccessful())
		return;

	sceneEnvironments.push_back(std::move(newSceneEnvironment));
}