#pragma once
#include "..\..\Include\datatypedef.h"
#include <map>
#include <vector>

class SceneObject;

class TagContainer
{
	std::multimap<u32, SceneObject*> m_tagHolder;

public:
	TagContainer();
	~TagContainer();
	void addObjectByTag(u32 tag, SceneObject* objectPointer);
	void getObjectsByTag(u32 tag, std::vector<SceneObject*>& objects);
};