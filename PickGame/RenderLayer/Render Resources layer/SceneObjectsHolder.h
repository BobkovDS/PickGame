#pragma once
#include "SceneObjectLayer.h"
#include "TagContainer.h"
#include <vector>

class SceneObjectsHolder
{
	std::vector<SceneObjectLayer> m_layers;
	TagContainer m_tagContainer;

	COPY_FORBID(SceneObjectsHolder)

public:

	SceneObjectsHolder();
	~SceneObjectsHolder();
	void getObjectsByTag(u32 tag, std::vector<SceneObject*>& objects);
	void addObject(SceneObject* object);
	void addObjectToLayer(LayerType type, SceneObject* object);
	SceneObjectLayer* getLayer(LayerType);
	void providePureLayer(LayerType type, std::vector<u32> tags, std::vector<SceneObject*>& objects);
};