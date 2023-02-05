#include "SceneObjectLayer.h"


SceneObjectLayer::SceneObjectLayer()
{
}

SceneObjectLayer::~SceneObjectLayer()
{
	u32 lenght = (u32)m_objects.size();
	for (u32 i = 0; i < lenght; i++)
		delete m_objects[i];
}

u32 SceneObjectLayer::provideInstances(std::vector<InstanceData>& instances)
{
	u32 instancesCountForThisLayer = 0;

	for (auto& object : m_objects)
		instancesCountForThisLayer += object->provideInstances(instances);
	
	return instancesCountForThisLayer;
}

u32 SceneObjectLayer::getInstancesCount()
{
	u32 instancesCountForThisLayer = 0;

	for (auto& object : m_objects)
		instancesCountForThisLayer += object->getInstancesCount();

	return instancesCountForThisLayer;
}
