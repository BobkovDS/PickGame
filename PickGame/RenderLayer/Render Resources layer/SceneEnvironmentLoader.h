#pragma once

#include "SceneEnvironment.h"

class ModelFileLoader;
class ResourceManager;

class SceneEnvironmentLoader
{
	ModelFileLoader* m_loader;

public:
	SceneEnvironmentLoader();
	~SceneEnvironmentLoader();

	void loadSceneEnvironmentFiles(
		std::vector<std::unique_ptr<SceneEnvironment>> &sceneEnvironments,
		ResourceManager* resourceManager);
};