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
	u16 getMaterialPackId() const { return m_materialPackId; }
	u16 getTexturePackId() const { return m_texturePackId; }

	SceneObjectsHolder& getObjectsHolder();
	void getAnimatedObjects(std::vector<SceneObject*>& objects);
};

struct SceneEnvironmentConnector
{
	bool IsNewData = false;
	SceneEnvironment* SceneEnvData = nullptr;
};