#pragma once
#include "SceneObjectsHolder.h"

class SceneEnvironment
{
	SceneObjectsHolder m_objects;
	u16 m_materialPackId;
	u16 m_texturePackId;

public:
	SceneEnvironment();

	void setMaterialPackId(u16 packId) { m_materialPackId = packId; }
	void setTexturePackId(u16 packId) { m_texturePackId = packId; }
	u16 getMaterialPackId() { return m_materialPackId; }
	u16 getTexturePackId() { return m_texturePackId; }

	SceneObjectsHolder& getObjectsHolder();
	void getAnimatedObjects(std::vector<SceneObject*>& objects);
};

struct SceneEnvironmentConnector
{
	bool IsNewData;
	SceneEnvironment* SceneEnvData;
};