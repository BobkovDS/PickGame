#pragma once
#include "SceneObjectsHolder.h"
#include <vector>

class SceneObjectLayerIterator
{
	std::vector<SceneObjectsHolder*> m_objectsHolders;
	u16 m_layersOffsetsByInstances[LayerType::LT_COUNT];

	u16 m_layersToShow; // Stores which Layers should be shown (1 - show)
	u16 m_currentLayerId;
public:
	SceneObjectLayerIterator();
	~SceneObjectLayerIterator();

	void addSceneObjectsHolder(SceneObjectsHolder* sceneObjectHolder);
	void build();
	void setVisibleLayers(u16 visibleLayersIDMask);
	void defineRequiredLayers(u16 layersIDMask);
	bool next(u32& offset, u8& layerID, std::vector<SceneObjectLayer*>& layers);
	void clear();
};