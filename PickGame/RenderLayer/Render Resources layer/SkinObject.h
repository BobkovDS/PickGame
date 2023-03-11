#pragma once
#include "SceneObjectsHolder.h"

class SkinObject
{
	std::string m_name;
	u16 m_materialPackId;
	u16 m_texturePackId;

	SceneObjectsHolder m_skinObjectsHolder;

	COPY_FORBID(SkinObject)
public:
	SkinObject();

	void setMaterialPackId(u16 packId) { m_materialPackId = packId; }
	void setTexturePackId(u16 packId) { m_texturePackId = packId; }
	u16 getMaterialPackId() { return m_materialPackId; }
	u16 getTexturePackId() { return m_texturePackId; }

	void getScore_digits(std::vector<SceneObject*>& objects);
	void getBoard_leftPart(std::vector<SceneObject*>& objects);
	void getBoard_rightPart(std::vector<SceneObject*>& objects);
	void getBoard_middlePart(std::vector<SceneObject*>& objects);
	void getBoard_pick(std::vector<SceneObject*>& objects);
	void getBoard_controlPad(std::vector<SceneObject*>& objects);
	void getAnimatedObjects(std::vector<SceneObject*>& objects);
	SceneObjectsHolder& getObjectsHolder();
};

struct SkinDataConnector
{
	bool IsNewSkin = false;
	SkinObject* SkinObjectData = nullptr;
};