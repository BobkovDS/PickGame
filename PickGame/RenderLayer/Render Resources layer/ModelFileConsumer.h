#pragma once
#include "..\..\Include\ModelFileLoader.h"

class ModelFileLoader;
class ResourceManager;
class SceneObject;
class SkinObject;
class PGFFileLoader;


class ModelFileConsumer: public ModelDataConsumer
{
	ResourceManager* _resourceManager;
	SkinObject* _newSkin;
	PGFFileLoader* _loader;

	SceneObject* buildSceneObject(u32 index);
	void writeMaterials();

	COPY_FORBID(ModelFileConsumer)
public:
	ModelFileConsumer(ResourceManager* resourceManager, SkinObject* newSkin);
	void consume(ModelFileLoader* fileLoader) override;
	bool writeToDevice() override;
};