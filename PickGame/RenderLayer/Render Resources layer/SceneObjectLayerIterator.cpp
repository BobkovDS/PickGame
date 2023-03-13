#include "SceneObjectLayerIterator.h"

SceneObjectLayerIterator::SceneObjectLayerIterator():
	m_layersToShow(0), // Shows no Layers by the default
	m_currentLayerId(0)
{
}

SceneObjectLayerIterator::~SceneObjectLayerIterator()
{
}

void SceneObjectLayerIterator::addSceneObjectsHolder(SceneObjectsHolder* sceneObjectsHolder)
{
	assert(sceneObjectsHolder);
	m_objectsHolders.push_back(sceneObjectsHolder);
}

void SceneObjectLayerIterator::build()
{

}

void SceneObjectLayerIterator::setVisibleLayers(u16 visibleLayersIDMask)
{

}

void SceneObjectLayerIterator::defineRequiredLayers(u16 layersIDMask)
{
	m_layersToShow = layersIDMask;
	m_currentLayerId = 0;
}

bool SceneObjectLayerIterator::next(u32& offset, u8& layerID, std::vector<SceneObjectLayer*>& layers)
{
	layers.clear();
	
	if (m_currentLayerId >= LayerType::LT_COUNT)
		return false;

	while (
		((m_layersToShow & (1 << m_currentLayerId)) == 0)
		&& (m_currentLayerId < LayerType::LT_COUNT))
	{
		/* Count Instances for invisible layers.
		Instances for visible layers will be count during drawing*/
		for (auto& holder : m_objectsHolders)
			offset += holder->getLayer(static_cast<LayerType>(m_currentLayerId))->getInstancesCount();

		m_currentLayerId++;
	}

	for (auto& holder : m_objectsHolders)
	{
		layers.push_back(holder->getLayer(static_cast<LayerType>(m_currentLayerId)));
	}

	m_currentLayerId++;

	return true;
}

void SceneObjectLayerIterator::clear()
{

}