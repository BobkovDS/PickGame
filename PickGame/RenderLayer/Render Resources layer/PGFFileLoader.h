#pragma once

#include <fstream>

#include "../../Include/ModelFileLoader.h"

class PGFFileLoader : public ModelFileLoader
{
	struct Mesh;
	struct Material;
	struct Instance;
	std::ifstream m_modelFile;

	std::vector<std::string> m_textures;
	std::vector<Mesh> m_meshes;
	std::vector<Material> m_materials;
	std::map<u32, std::vector<Instance>> m_instancesByMeshId;

	void readMeshData();
	void readMaterialData();
	void readInstancesData();
	void readTextureData();

public:
	void loadFile(const std::string& fileName) override;
	void prepare();

	u32 meshCount() const override{ return m_meshes.size(); }
	u32 materialsCount() const { return m_materials.size(); }
	u32 instancesCount(u32 meshId = 0) const override;
	u32 texturesCount() const override { return m_textures.size(); }

	const Mesh* mesh(u32 i) const
	{
		return dataVector<Mesh>(i, m_meshes);
	}

	const Material* material(int i) const
	{
		return dataVector<Material>(i, m_materials);
	}

	const std::vector<Instance>& instancesForMesh(u32 meshId) const
	{
		auto it = m_instancesByMeshId.find(meshId);
		if (it != m_instancesByMeshId.end())
			return it->second;

		return std::vector<Instance>(); //TODO: is it OK?
	}
	
	const std::string& texture(int i)
	{
		return m_textures[i % m_textures.size()];
	}

	struct Mesh
	{
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
		int MatCBIndex;
		DirectX::XMFLOAT4 DiffuseAlbedo;
		DirectX::XMFLOAT4 TransparencyColor;
		DirectX::XMFLOAT3 FresnelR0;
		DirectX::XMFLOAT4X4 MaterialTransformation;
		u32 Flags;
		u32 TextureMapIDs[6];
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
		u32 MeshID;
		
		bool Visible;
		std::vector<u32> MaterialIDs;
		DirectX::XMFLOAT3 Transformation;
		DirectX::XMFLOAT3 Translation;
		DirectX::XMFLOAT4X4 GlobalTransformation;
	};
};