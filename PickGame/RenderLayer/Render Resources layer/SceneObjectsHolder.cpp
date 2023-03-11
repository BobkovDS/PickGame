#include "SceneObjectsHolder.h"

SceneObjectsHolder::SceneObjectsHolder()
{
	m_layers.resize( static_cast<u8>(LayerType::LT_COUNT));
}

SceneObjectsHolder::~SceneObjectsHolder()
{
	m_layers.clear(); // TO_DO: check how it works
}

void SceneObjectsHolder::addObject(SceneObject* object)
{
	if (object->getType() == SceneObjectType::SOT_Opaque)
		addObjectToLayer(LayerType::LT_opaque, object);
}

void SceneObjectsHolder::addObjectToLayer(LayerType type, SceneObject* object)
{
	m_layers[type].addSceneObject(object);

	for (u32 tag : object->getTags())
	{
		m_tagContainer.addObjectByTag(tag, object);
	}
}

void SceneObjectsHolder::getObjectsByTag(u32 tag, std::vector<SceneObject*>& objects)
{
	m_tagContainer.getObjectsByTag(tag, objects);
}

SceneObjectLayer* SceneObjectsHolder::getLayer(LayerType type)
{
	return &m_layers[type];
}