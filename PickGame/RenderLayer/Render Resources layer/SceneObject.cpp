#include "SceneObject.h"
#include <assert.h>

using namespace std;

SceneObject::SceneObject():
	m_instancesID_Size(0)
{
	m_isBkupInstancesUsed = false;
	m_doesItOwnMesh = false;
	m_mesh = nullptr;
	for (u8 i = 0; i < LOD_COUNT; i++)
	{
		m_mesh_LOD[i] = nullptr;
		m_instancesID_LOD_size[i] = 0;
		m_drawArgsLOD[i].first = nullptr;
		m_drawArgsLOD[i].second = 0;
	}
}

SceneObject::~SceneObject()
{
	if (m_doesItOwnMesh)
	{
		if (m_mesh)
			delete m_mesh;

		for (u8 i = 0; i < LOD_COUNT; i++)
			if (m_mesh_LOD[i])
				delete m_mesh_LOD[i];
	}
}

//SceneObject& SceneObject::operator=(const SceneObject& cp)
//{
//	m_name = cp.m_name;
//	m_type = cp.m_type;
//	m_tags = cp.m_tags;
//	m_instanceBkUp = cp.m_instanceBkUp;
//	m_instances = cp.m_instances;
//	m_mesh = cp.m_mesh;
//	m_isBkupInstancesUsed = cp.m_isBkupInstancesUsed;
//
//	for (u8 i = 0; i < LOD_COUNT; i++)
//	{
//		m_instancesID_LOD[i] = cp.m_instancesID_LOD[i];
//		m_instancesID_LOD_size[i] = cp.m_instancesID_LOD_size[i];
//		m_mesh_LOD[i] = cp.m_mesh_LOD[i];
//	}
//	m_doesItOwnMesh = false; // Copied object does not own Mesh object(s)
//
//	return *this;
//}

Mesh* SceneObject::createMesh()
{
	if (m_mesh == nullptr)
	{
		m_doesItOwnMesh = true;
		m_mesh = new Mesh;
	}
	
	return m_mesh;
}

Mesh* SceneObject::createMesh_LOD(u8 LODid)
{
	LODid %= LOD_COUNT;

	if (m_mesh_LOD[LODid] == nullptr)
	{
		m_doesItOwnMesh = true;
		m_mesh_LOD[LODid] = new Mesh;
	}

	return m_mesh_LOD[LODid];
}

void SceneObject::makeInstancesBkup()
{
	assert(m_instances.size() == 1);
	m_instanceBkUp = m_instances[0];
}