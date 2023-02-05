#include "SceneEnvironment.h"
#include "../../../FBXLoader/StringIDs.h"

SceneEnvironment::SceneEnvironment():
	m_materialPackId(U16_MAX),
	m_texturePackId(U16_MAX)
{
}

SceneObjectsHolder& SceneEnvironment::getObjectsHolder()
{
	return m_objects;
}

void SceneEnvironment::getAnimatedObjects(std::vector<SceneObject*>& objects)
{	
	m_objects.getObjectsByTag(SKIN_ANIM_EFFECT, objects);	
}