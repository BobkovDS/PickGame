#pragma once
#include "SkinObject.h"

class ModelFileLoader;
class ResourceManager;

class SkinLoader
{
	ModelFileLoader* m_loader;

public:
	SkinLoader();
	~SkinLoader();

	void loadSkinFiles(std::vector<std::unique_ptr<SkinObject>>&, ResourceManager* resourceManager);
};