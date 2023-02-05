#pragma once

#include "StringIDs.h"
#include "StringConstIDs.h"
#include "..\PickGame\Include\ModelFileLoader.h"

namespace FBXSDK_NAMESPACE
{
	class FbxScene;
	class FbxNode;
	class FbxSurfacePhong;
	class FbxProperty;
	class FbxMesh;
	class FbxMatrix;
}

namespace FBXLoader
{
	struct TreeBoneNode
	{
		std::string Name;
		FBXSDK_NAMESPACE::FbxNode* Node;
		std::vector<FBXSDK_NAMESPACE::FbxNode*> Childs;
	};
}

class FBXFileLoader : public ModelFileLoader
{
	struct Instance;
	struct Material;
	struct Mesh;

	std::vector<Instance> m_instances;
	std::map<std::string, Material> m_materials;
	std::map<std::string, Mesh> m_meshes;

	// mapping
	std::map<std::string, std::string> m_texturePathByName;
	std::vector<std::string> m_textureIdByPath; // position of path in vector is Texture ID
	std::map<const Material*, u32> m_materialIDByPtr;
	std::map<std::string, u32> m_meshIdByName;

	FBXSDK_NAMESPACE::FbxScene* m_scene;

	void createScene();

	void processNodes();
	void processEachNode(FBXSDK_NAMESPACE::FbxNode* pNode);
	void processNodeMaterial(FBXSDK_NAMESPACE::FbxNode* pNode, FBXFileLoader::Instance& inst);
	void processNodeSkeleton(FBXSDK_NAMESPACE::FbxNode* pNode, FBXFileLoader::Instance& inst);
	void processNodeMesh(FBXSDK_NAMESPACE::FbxNode* pNode, FBXFileLoader::Instance& inst);
	void processNodeCamera(FBXSDK_NAMESPACE::FbxNode* pNode, FBXFileLoader::Instance& inst);
	void processNodeLight(FBXSDK_NAMESPACE::FbxNode* pNode, FBXFileLoader::Instance& inst);
	void processNodeLODGroup(FBXSDK_NAMESPACE::FbxNode* pNode, FBXFileLoader::Instance& inst);

	
	void readMaterialCustomProperties(FBXSDK_NAMESPACE::FbxSurfacePhong* phongMaterial, FBXFileLoader::Material& material);
	void readMaterialDefaultProperties(FBXSDK_NAMESPACE::FbxSurfacePhong* phongMaterial, FBXFileLoader::Material& material);

	bool readTextureInfo(FBXSDK_NAMESPACE::FbxProperty* fbxProperty, TEXTURE_TYPE textureType, FBXFileLoader::Material& material);
	bool readMeshInfo(FBXSDK_NAMESPACE::FbxMesh* pMesh, FBXFileLoader::Mesh& newMesh, bool tessellated = false);
	bool readMeshInfo_UV3(const FBXSDK_NAMESPACE::FbxMesh* pMesh, FBXFileLoader::Mesh& newMesh);

	void convertFBXMatrixToFloat4X4(const FBXSDK_NAMESPACE::FbxMatrix& fbxM, DirectX::XMFLOAT4X4& m4x4);

	void addTexturePathByName_(const std::string& name, const std::string& path);

	void buildTextureIDList()
	{
		/*************
		Create a list of [TexturePath->TextureID].
		Texture's paths are unique for all textures. So later we will upload on GPU each texture only once.
		Materials will use this list to map their Texture_name with Texture_ID (see textureIDByName() method),
		where Texture_ID is position of this texture in Texture array on GPU.
		*************/

		/* Build unique texture names list and set ID for it:
		* 1) Take Texture Path from map[name->path]
		* 2) Check whether this path exist in vector<Path>
		* 3) If not, add to vector<Path>; Position of Path in this vector is Texure ID
		*/
		{
			for (auto& pathAndName : m_texturePathByName)
			{
				auto it = std::find(m_textureIdByPath.begin(), m_textureIdByPath.end(), pathAndName.first);
				if (it == m_textureIdByPath.end())
				{
					m_textureIdByPath.push_back(pathAndName.first);
				}
			}
		}
	}

	void buildMeshsIDByNameList()
	{
		/*
		*	Build a list of [Mesh_Name -> Mesh_ID]
		*/

		u32 meshId = 0;
		for (u32 i = 0; i < meshCount(); i++)
		{
			auto meshName = mesh(i)->Name;
			if (m_meshIdByName.find(meshName) != m_meshIdByName.end())
				assert(0 && "Mesh with such name exists");

			m_meshIdByName[meshName] = meshId++;
		}
	}

	void buildMaterialsIDList()
	{
		/*
		*	Build a list of [Material_Ptr->Material_ID]
		*/
		u32 materialID = 0;

		for (u32 i = 0; i < materialsCount(); i++)
		{
			auto mptr = material(i);
			if (mptr == nullptr)
				continue;

			m_materialIDByPtr[mptr] = materialID++;
		}
	}

public:
	FBXFileLoader();
	~FBXFileLoader();

	void loadFile(const std::string& skinFileName) override;
	void prepare() override;

	const Mesh* mesh(int i) const
	{
		return data<Mesh>(i, m_meshes);
	}

	const Material* material(int i) const
	{
		return data<Material>(i, m_materials);
	}

	const Instance* instance(int i) const
	{
		return dataVector(i, m_instances);
	}

	u32 meshCount() const override { return m_meshes.size(); }
	u32 materialsCount() const override { return m_materials.size(); }
	u32 instancesCount(u32 meshId = 0) const override { return m_instances.size(); }
	u32 texturesCount() const override { return m_textureIdByPath.size(); }
	
	const std::string& texturePath(u32 id) const
	{
		std::string textureName = m_textureIdByPath[id % m_textureIdByPath.size()];
		auto it = m_texturePathByName.find(textureName);
		assert(it != m_texturePathByName.end());
		return it->second;
	}

	u32 textureIDByName(const std::string& name) const
	{
		/************
			What we do:
				TextureName->|[m_texturePathByName]|->TexturePath->|[m_textureIDByPath]|->TextureID
		*************/

		auto it1 = m_texturePathByName.find(name);
		assert(it1 != m_texturePathByName.end());

		auto it2 = std::find(m_textureIdByPath.begin(), m_textureIdByPath.end(), name);
		assert(it2 != m_textureIdByPath.end());

		return std::distance(m_textureIdByPath.begin(), it2);
	}

	u32 materialIDByPtr(Material* ptr) const
	{
		auto it = m_materialIDByPtr.find(ptr);
		if (it != m_materialIDByPtr.end())
			return it->second;

		return 0;
	}

	u32 meshIDByName(const std::string& meshName) const
	{
		auto it = m_meshIdByName.find(meshName);
		if (it != m_meshIdByName.end())
			return it->second;

		return 0;
	}

	enum NodeType
	{
		NT_Mesh = 'mesh',
		NT_Light,
		NT_Camera
	};

	struct Mesh
	{
		std::string Name;
		std::vector<u32> Indices;
		std::vector<DirectX::XMFLOAT3> Vertices;
		std::vector<std::vector<std::pair<std::string, float>>> VertexWeightByBoneName;
		std::vector<DirectX::XMFLOAT3> Normals;
		std::vector<DirectX::XMFLOAT3> Tangents;
		std::vector<DirectX::XMFLOAT3> BiTangents;
		std::vector<DirectX::XMFLOAT2> UVs;
		bool WasUplaod;
		bool ExcludeFromCulling;
		bool ExcludeFromMirrorReflection;
		bool DoNotDublicateVertices;
		bool MaterialHasNormalTexture;
		int VertexPerPolygon;
		Mesh()
		{
			WasUplaod = false;
			ExcludeFromCulling = false;
			ExcludeFromMirrorReflection = false;
			DoNotDublicateVertices = false;
			MaterialHasNormalTexture = false;
			VertexPerPolygon = 0;

			Indices.clear();
		}
	};

	struct Material
	{
		std::string Name;
		int MatCBIndex;
		DirectX::XMFLOAT4 DiffuseAlbedo;
		DirectX::XMFLOAT4 Specular;
		DirectX::XMFLOAT4 TransparencyColor;
		DirectX::XMFLOAT3 FresnelR0;
		DirectX::XMFLOAT4X4 MaterialTransformation;
		std::vector<std::pair<TEXTURE_TYPE, std::string>> TextureNameType;
		float Roughness;


		bool isTransparencyUsed;
		bool isTransparent;
		bool isSky;
		bool doesIncludeToWorldBB;
		bool doesItHaveNormalTexture;

		Material() :
			MatCBIndex(-1),
			FresnelR0({ 0.5f, 0.1f, 0.1f }),
			Roughness(0.1f)
		{}
	};

	struct Instance
	{
		std::string MeshName;
		NodeType NodeType;
		bool Visible;
		std::vector<Material*> Materials;
		DirectX::XMFLOAT3 Transformation;
		DirectX::XMFLOAT3 Translation;
		DirectX::XMFLOAT4X4 GlobalTransformation;
	};
};

// SOME UTILS

struct D4toF4
{
	struct D4{
		double d3;
		double* ptr;
	};
	
	union {
		D4 d4;
		double m[4];
	} un;

	DirectX::XMFLOAT4 toF4()
	{
		return DirectX::XMFLOAT4((float) un.m[1], (float) un.m[2], (float)un.m[3], (float)un.m[0]);
	}

	D4toF4(double* ptr, double d3)
	{
		un.d4.ptr = ptr;
		un.d4.d3 = d3;
	}
};


template<typename V, typename T>
void FBXArrayToXMFLOATVector(const T& inArray, std::vector<V>& outVector, int index, int count = 3)
{
	for (int i = 0; i < count; i++)
	{
		outVector.push_back(V(
			inArray.GetAt(index + i).mData[0],
			inArray.GetAt(index + i).mData[1],
			inArray.GetAt(index + i).mData[2])
		);
	}
}


/*
* Build a Quad from 4 vertices.
	if this Quad face for Tesselated mesh, so no need to subdevide it now. In other case, devide it to two triangles.
*/
template<typename T>
void BuildQuad(T t0, T t1, T t2, T t3, std::vector<T>& outVector, bool isSimpleQuad=true)
{
	if (!isSimpleQuad)
	{
		outVector.push_back(t0);
		outVector.push_back(t1);
		outVector.push_back(t3);
		outVector.push_back(t2);
	}
	else
	{
		outVector.push_back(t0);
		outVector.push_back(t1);
		outVector.push_back(t2);

		outVector.push_back(t0);
		outVector.push_back(t2);
		outVector.push_back(t3);
	}
}

/*
*	Using input FBX array, read and build a Quad from 4 points. If it is simple Quad,
*	so just build two Triangles. If it is Quad for tesselation, write just four vertices
*/
template<typename A, typename V>
void FBXQuadArrayToVector(const A& inArray, V& outVector, int index, bool isSimpleQuad)
{
	DirectX::XMFLOAT3 vertices[4];

	for (int i = 0; i < 4; i++)
	{
		vertices[i] = DirectX::XMFLOAT3(
			inArray.GetAt(index + i).mData[0],
			inArray.GetAt(index + i).mData[1],
			inArray.GetAt(index + i).mData[2]
		);
	}
	
	BuildQuad<DirectX::XMFLOAT3>(vertices[0], vertices[1], vertices[2], vertices[3], outVector, isSimpleQuad);
}