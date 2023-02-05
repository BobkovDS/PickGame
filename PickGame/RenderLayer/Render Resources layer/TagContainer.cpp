#include "TagContainer.h"
#include "SceneObject.h"

TagContainer::TagContainer()
{
}

TagContainer::~TagContainer()
{
}

void TagContainer::addObjectByTag(u32 tag, SceneObject* objectPointer)
{
	m_tagHolder.insert(std::make_pair(tag, objectPointer));
}

void TagContainer::getObjectsByTag(u32 tag, std::vector<SceneObject*>& objects)
{
	auto it = m_tagHolder.equal_range(tag);

	for (auto i = it.first; i != it.second; ++i)
	{
		objects.push_back(i->second);
	}
}