#include "SkinObject.h"
#include "../../../FBXLoader/StringIDs.h"
using namespace std;

SkinObject::SkinObject():
	m_materialPackId(U16_MAX),
	m_texturePackId(U16_MAX)
{
}

void SkinObject::getScore_digits(vector<SceneObject*>& objects)
{
	m_skinObjectsHolder.getObjectsByTag(SKIN_DIGITS_PART, objects);
}

void SkinObject::getBoard_leftPart(vector<SceneObject*>& objects)
{
	m_skinObjectsHolder.getObjectsByTag(SKIN_LEFT_PART, objects);
}

void SkinObject::getBoard_rightPart(vector<SceneObject*>& objects)
{
	m_skinObjectsHolder.getObjectsByTag(SKIN_RIGHT_PART, objects);
}

void SkinObject::getBoard_middlePart(vector<SceneObject*>& objects)
{
	m_skinObjectsHolder.getObjectsByTag(SKIN_MIDDLE_PART, objects);
}

void SkinObject::getBoard_pick(vector<SceneObject*>& objects)
{
	m_skinObjectsHolder.getObjectsByTag(SKIN_PICK, objects);
}

void SkinObject::getBoard_controlPad(vector<SceneObject*>& objects)
{
	m_skinObjectsHolder.getObjectsByTag(SKIN_CTRL_PAD, objects);
}

void SkinObject::getAnimatedObjects(vector<SceneObject*>& objects)
{
	m_skinObjectsHolder.getObjectsByTag(SKIN_ANIM_EFFECT, objects);
}

SceneObjectsHolder& SkinObject::getObjectsHolder()
{
	return m_skinObjectsHolder;
}