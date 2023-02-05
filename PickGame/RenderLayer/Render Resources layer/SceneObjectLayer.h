#pragma once
#include "SceneObject.h"
#include <vector>

class SceneObjectLayer
{
	std::vector<SceneObject*> m_objects;
	
	SceneObjectLayer& operator=(const SceneObjectLayer&) = delete;
public:
	SceneObjectLayer();
	~SceneObjectLayer();

	void addSceneObject(SceneObject* object);
	const std::vector<SceneObject*>& getSceneObjects();
	u32 provideInstances(std::vector<InstanceData>& instances);
	u32 getInstancesCount();
};

inline void SceneObjectLayer::addSceneObject(SceneObject* object)
{
	m_objects.push_back(object);
}

inline const std::vector<SceneObject*>& SceneObjectLayer::getSceneObjects()
{
	return m_objects;
}