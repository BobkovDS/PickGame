#pragma once
#include "..\..\Include\datatypedef.h"
#include "..\..\Include\enums.h"
#include "gpuStructures_mesh.h"
#include <vector>

class SceneObject
{
	std::string m_name; // ? do we need it?
	SceneObjectType m_type;
	std::vector<u32> m_tags;
	InstanceData m_instanceBkUp; // bkup for the first Instance
	std::vector<InstanceData> m_instances; // all Instances for this mesh in a Scene
	std::vector<u32> m_instancesID; // m_instancesID - contains IDs of selected m_instancs to be drawn.
	std::vector<u32> m_instancesID_LOD[LOD_COUNT]; //m_instancesID_LOD[x] - contains Instances IDs for Mesh with LOD x. Instances ID point to m_instances
	u32 m_instancesID_Size; // size of m_instancesID
	u32 m_instancesID_LOD_size[LOD_COUNT]; // m_instancesID_LOD_size[x] = m_instancesID_LOD[x].size()
	std::pair<Mesh*, u32> m_drawArgs;
	std::pair<Mesh*, u32> m_drawArgsLOD[LOD_COUNT];
	Mesh* m_mesh; // A Mesh belongs to one SceneObject only. So it can be safely created and deleted by this SceneObject
	Mesh* m_mesh_LOD[LOD_COUNT];
	bool m_isBkupInstancesUsed;
	bool m_doesItOwnMesh;

	void getInstancesLOD_(std::vector<InstanceData>& outInstancesLOD, u8 LODId);

	COPY_FORBID(SceneObject)
public:
	SceneObject();

	~SceneObject();
	Mesh* createMesh(); // create a mesh (only once)
	Mesh* createMesh_LOD(u8 lodId); // create a LOD mesh (only once)
	const Mesh* getMesh() const;
	const Mesh* getMesh_LOD(u8 lodId) const;
	const InstanceData* const getInstanceBkup() const;

	void setName(std::string& name);
	const std::string& getName() const;
	
	void setType(SceneObjectType type);
	SceneObjectType getType() const;

	void addTag(u32 tag);
	const std::vector<u32>& getTags() const;

	void addInstance(const InstanceData& instance);
	const std::vector<InstanceData>& getInstances();
	u32 provideInstances(std::vector<InstanceData>& instances);
	u32 getInstancesCount();
	void clearInstances();
	void clearInstancesLOD();
	void setInstancesLODCount();
	void makeInstancesBkup();
	void addInstanceID(u32 instanceId, u8 LODId);
	
	/*provides how many Meshes to draw for this SceneObject (just One mesh or
		LOD meshes) and Instances count for each Mesh*/
	std::pair<Mesh*, u32>* SceneObject::getDrawArgs(u8& meshCount);
};

inline void SceneObject::setName(std::string& name)
{
	m_name = name;
}

inline const std::string& SceneObject::getName() const
{
	return m_name;
}

inline void SceneObject::setType(SceneObjectType type)
{
	m_type = type;
}

inline SceneObjectType SceneObject::getType() const
{
	return m_type;
}

inline void SceneObject::addTag(u32 tag)
{
	m_tags.push_back(tag);
}

inline const std::vector<u32>& SceneObject::getTags() const
{
	return m_tags;
}

inline void SceneObject::addInstance(const InstanceData& instance)
{
	m_instances.push_back(instance);

	// TODO: TEMP!!!

	m_instancesID.push_back(m_instances.size() - 1);
	m_instancesID_Size++;
}

inline const std::vector<InstanceData>& SceneObject::getInstances()
{
	return m_instances;
}

inline u32 SceneObject::provideInstances(std::vector<InstanceData>& instances)
{
	/*
	*	Provide all instances which should be drawn for this Scene object.
	*/

	u32 currentCount = instances.size();

	if (m_mesh)
	{
		instances.resize(currentCount + m_instancesID_Size);

		for (u32 i = 0; i < m_instancesID_Size; i++)
			instances[currentCount + i] = m_instances[m_instancesID[i]];

		return m_instancesID_Size;
	}
	else
	{
		for (u8 i = 0; i < LOD_COUNT; i++)
			getInstancesLOD_(instances, i);

		return instances.size() - currentCount;
	}
}

inline u32 SceneObject::getInstancesCount()
{
	if (m_mesh)
		return m_instancesID_Size;
	else
	{
		u32 count = 0;
		for (u8 i = 0; i < LOD_COUNT; i++)
			count += m_instancesID_LOD_size[i];

		return count;
	}
}

inline const InstanceData* const SceneObject::getInstanceBkup() const
{
	return &m_instanceBkUp;
}

inline void SceneObject::clearInstances()
{
	m_instances.clear(); // or m_instances.resize(0); ??

	m_instancesID.clear();
	m_instancesID_Size = 0;
}

inline void SceneObject::clearInstancesLOD()
{
	for (u8 i = 0; i < LOD_COUNT; i++)
		m_instancesID_LOD_size[i] = 0;

	// We need to check a size of m_instancesID_LOD[i] every time for 'extended' SceneObject object, 
	// cause m_instances size can be dynamicly changed
	if (m_isBkupInstancesUsed)
		setInstancesLODCount();
}

inline void SceneObject::addInstanceID(u32 instanceId, u8 LODid)
{
	LODid %= LOD_COUNT; // just to be safe
	m_instancesID_LOD[LODid][m_instancesID_LOD_size[LODid]++] = instanceId;
}

inline void SceneObject::setInstancesLODCount()
{
	for (u8 i = 0; i < LOD_COUNT; i++)
		if (m_instancesID_LOD[i].size() < m_instances.size())
			m_instancesID_LOD[i].resize(m_instances.size());
}

inline const Mesh* SceneObject::getMesh() const
{
	return m_mesh;
}

inline const Mesh* SceneObject::getMesh_LOD(u8 lodId) const
{
	lodId %= LOD_COUNT;
	return m_mesh_LOD[lodId];
}

inline std::pair<Mesh*, u32>* SceneObject::getDrawArgs(u8& meshCount)
{
	/*
		- If we have m_mesh is set, we think that this SceneObject does not use LODs. So we return that we
		need to draw only one mesh and how many Instances it has

		- If SceneObject has LOD meshes, we return that we need draw all this LODs, but how many Instances has exactly
		each LOD says instancesCountByLOD (it can be 0 also)
	*/

	if (m_mesh)
	{
		meshCount = 1;
		m_drawArgs.first = m_mesh;
		m_drawArgs.second = m_instancesID_Size;

		return &m_drawArgs;
	}
	else
	{
		meshCount = LOD_COUNT;

		for (u8 i = 0; i < LOD_COUNT; i++)
		{
			m_drawArgsLOD[i].first = m_mesh_LOD[i];
			m_drawArgsLOD[i].second = m_instancesID_LOD_size[i];
		}

		return m_drawArgsLOD;
	}
}

inline void SceneObject::getInstancesLOD_(std::vector<InstanceData>& outInstances, u8 LODId)
{
	// Provides InstancesData to copy it to GPU

	u8 safeLODId = LODId % LOD_COUNT;
	u32 instancesLODCount = m_instancesID_LOD_size[safeLODId];
	u32 currentInstanceCount = (u32)outInstances.size();
	outInstances.resize(currentInstanceCount + instancesLODCount);

	for (u32 i = 0; i < instancesLODCount; i++)
		outInstances[currentInstanceCount + i] = m_instances[m_instancesID_LOD[safeLODId][i]];
}