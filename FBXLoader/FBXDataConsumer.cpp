#include "FBXDataConsumer.h"
#include "FBXFileLoader.h"
#include "Logger.h"

#include "StringIDs.h"
#include "StringConstIDs.h"

using namespace std;

template<typename T, typename C>
void printBuffer(std::ofstream& file, C& c)
{
	u32 count = c.size();
	file.write(reinterpret_cast<const char*>(&count), 4);

	if (count == 0)
		return;

	u32 size = sizeof(T) * c.size();
	file.write(reinterpret_cast<const char*>(&size), 4);
	file.write(reinterpret_cast<const char*>(c.data()), size);
}

struct MeshWriter
{
	const FBXFileLoader::Mesh* m_mesh;

	MeshWriter(const FBXFileLoader::Mesh* mesh);
	friend void operator<<(std::ofstream& file, const MeshWriter& mw);
};

struct InstancesWriter
{
	const FBXFileLoader::Instance* m_instance;
	const FBXFileLoader* m_loader;

	InstancesWriter(const FBXFileLoader* m_loader, const FBXFileLoader::Instance* instance);
	friend void operator<<(std::ofstream& file, const InstancesWriter& mw);
};

struct MaterialWriter
{
	const FBXFileLoader::Material* m_material;
	const FBXFileLoader* m_loader;

	MaterialWriter(const FBXFileLoader* loader, const FBXFileLoader::Material* material);
	friend void operator<<(std::ofstream& file, const MaterialWriter& mw);
};

static void writeString(std::ofstream& stream, const std::string& str)
{
	u32 size = str.size();
	stream.write(reinterpret_cast<char*>(&size), 4);
	stream.write(str.c_str(), str.size());
}

void FBXDataConsumer::consume(ModelFileLoader* fileLoader)
{
	assert(fileLoader);

	m_loader = dynamic_cast<FBXFileLoader*>(fileLoader);
	assert(m_loader);

	m_loader->prepare();
}

bool FBXDataConsumer::writeToDevice()
{
	/* true - ok, false - error */

	if (prepareOutputFile_())
	{
		writeMeshes_();

		writeMaterials_();

		writeInstances_();

		m_outFile.close();

		return true;
	}
	
	return false;
}

bool FBXDataConsumer::prepareOutputFile_()
{
	auto sceneName = m_loader->sceneName();

	auto lastSlash = sceneName.find_last_of("/");
	
	lastSlash = lastSlash != std::string::npos && lastSlash != sceneName.size() - 1 ? ++lastSlash : std::string::npos;

	if (lastSlash != std::string::npos)
		sceneName = sceneName.substr(lastSlash);

	auto fileName = sceneName + ".pgf";

	m_outFile.open(fileName, ios_base::binary);

	return m_outFile.is_open();
}

void FBXDataConsumer::writeMeshes_()
{
	// write DATAID
	string dataID("MESH");

	dataID.insert(dataID.end(), 6, '*');
	m_outFile.write(dataID.c_str(), dataID.size());


	// write Meshes count
	const u32 count = m_loader->meshCount();
	m_outFile.write(reinterpret_cast<const char*>(&count), 4);

	// write Meshes
	for (int i = 0; i < count; i++)
		m_outFile << MeshWriter(m_loader->mesh(i));

}

void FBXDataConsumer::writeTextures()
{
	// write DATAID
	string dataID("TEXTURES");

	dataID.insert(dataID.end(), 10 - dataID.size(), '*');
	m_outFile.write(dataID.c_str(), dataID.size());

	// Write texture [id: texturePath]
	{
		u32 textureCount = m_loader->texturesCount();
		m_outFile.write(reinterpret_cast<char*>(&textureCount), sizeof textureCount);
		
		for (u32 i = 0; i < m_loader->texturesCount(); i++)
			writeString(m_outFile, m_loader->texturePath(i));
	}
}

void FBXDataConsumer::writeMaterials_()
{
	// Write Texutures Path.
	writeTextures();

	// write DATAID
	string dataID("MATERIALS");

	dataID.insert(dataID.end(), 10 - dataID.size(), '*');
	m_outFile.write(dataID.c_str(), dataID.size());

	auto count = m_loader->materialsCount();
	m_outFile.write(reinterpret_cast<char*>(&count), sizeof count);

	for (int i = 0; i < count; i++)
	{
		m_outFile << MaterialWriter(m_loader, m_loader->material(i));
	}
}

void FBXDataConsumer::writeInstances_()
{
	// Notion: This Instances with Mesh data. No Camera and Light

	// Preparation
	//m_loader->buildMeshsIDByNameList();
	//m_loader->buildMaterialsIDList();

	// write DATAID
	string dataID("INSTANCES");

	dataID.insert(dataID.end(), 10 - dataID.size(), '*');
	m_outFile.write(dataID.c_str(), dataID.size());

	// write Instances count
	const u32 count = m_loader->instancesCount();
	m_outFile.write(reinterpret_cast<const char*>(&count), 4);

	for (int i = 0; i < count; i++)
		m_outFile << InstancesWriter(m_loader, m_loader->instance(i));

}

MeshWriter::MeshWriter(const FBXFileLoader::Mesh* _mesh)
{
	assert(_mesh && "NULL for MeshWriter()");

	m_mesh = _mesh;
}

void operator<<(std::ofstream& file, const MeshWriter& mw)
{
	if (mw.m_mesh == nullptr) return;

	// Mesh Name size
	{
		u32 size = mw.m_mesh->Name.size();
		file.write(reinterpret_cast<const char*>(&size), 4);
	}
	
	// Mesh Name value
	{
		file.write(mw.m_mesh->Name.c_str(), mw.m_mesh->Name.size());
	}

	// Indices buffer size & Indices buffer
	{		
		printBuffer<u32>(file, mw.m_mesh->Indices);
	}

	// Vertices buffer size & Vertices buffer
	{
		printBuffer<DirectX::XMFLOAT3>(file, mw.m_mesh->Vertices);
	}

	// Normal buffer size & Normal buffer
	{
		printBuffer<DirectX::XMFLOAT3>(file, mw.m_mesh->Normals);
	}

	// Tangents buffer size & Tangents buffer
	{
		printBuffer<DirectX::XMFLOAT3>(file, mw.m_mesh->Tangents);
	}

	// BiTangents buffer size & BiTangents buffer
	{
		printBuffer<DirectX::XMFLOAT3>(file, mw.m_mesh->BiTangents);
	}

	// UVs buffer size & UVs buffer
	{
		printBuffer<DirectX::XMFLOAT2>(file, mw.m_mesh->UVs);
	}
	
	// Flags
	{
		char flags[4] =
		{
			mw.m_mesh->ExcludeFromCulling,
			mw.m_mesh->ExcludeFromMirrorReflection,
			mw.m_mesh->DoNotDublicateVertices,
			mw.m_mesh->MaterialHasNormalTexture
		};

		file.write(flags, sizeof flags);
	}
}

InstancesWriter::InstancesWriter(const FBXFileLoader* loader, const FBXFileLoader::Instance* instance)
{
	assert(loader);
	assert(instance);
	
	m_loader = loader;
	m_instance = instance;
}

void operator<<(std::ofstream& file, const InstancesWriter& iw)
{
	if (iw.m_loader == nullptr || iw.m_instance == nullptr)
		return;

	// Mesh ID
	{
		auto value = iw.m_loader->meshIDByName(iw.m_instance->MeshName);
		file.write(reinterpret_cast<char*>(&value), sizeof(value));
	}

	// Visible
	{
		char value = iw.m_instance->Visible;
		file.write(&value, sizeof(value));
	}

	// Materials ID
	{
		u32 materialCount = iw.m_instance->Materials.size();
		file.write(reinterpret_cast<char*>(&materialCount), sizeof(materialCount));

		for (u8 i = 0; i < materialCount; i++)
		{
			auto materialId = iw.m_loader->materialIDByPtr(iw.m_instance->Materials[i]);
			file.write(reinterpret_cast<char*>(&materialId), sizeof(materialId));
		}
	}

	// Transformation
	{
		auto value = iw.m_instance->Transformation;
		file.write(reinterpret_cast<char*>(&value), sizeof(value));
	}

	// Transformation
	{
		auto value = iw.m_instance->Translation;
		file.write(reinterpret_cast<char*>(&value), sizeof(value));
	}

	// GlobalTransformation
	{
		auto value = iw.m_instance->GlobalTransformation;
		file.write(reinterpret_cast<const char*>(&iw.m_instance->GlobalTransformation), sizeof(iw.m_instance->GlobalTransformation));
	}
}

MaterialWriter::MaterialWriter(const FBXFileLoader* loader, const FBXFileLoader::Material* material)
{
	assert(material);
	assert(loader);

	m_material = material;
	m_loader = loader;
}

void operator<<(std::ofstream& file, const MaterialWriter& mw)
{
	if (mw.m_material == nullptr || mw.m_loader == nullptr) return;

	//DiffuseAlbedo + Transparency Factor
	{
		auto value = mw.m_material->DiffuseAlbedo;
		value.w = mw.m_material->TransparencyColor.w;
		file.write(reinterpret_cast<char*>(&value), sizeof(value));
	}

	//FresnelR0
	{
		auto value = mw.m_material->FresnelR0;
		file.write(reinterpret_cast<char*>(&value), sizeof(value));
	}

	//MaterialTransformation
	{
		auto value = mw.m_material->MaterialTransformation;
		file.write(reinterpret_cast<char*>(&value), sizeof(value));
	}

	// Texture IDs
	{
		/*We will convert this information, which is ready to read for PGF loader or application.
			We need this information in next way:
			- Flags[u32]: shows which tetures are used
				0 - Diffuse texture ¹1
				1 - Diffuse texture ¹2
				2 - Normal texture
				3 - Specular texture
				4 - Transperency factor texture
				5 - None
			- TextureMapIndex[6] [u32[6]]: Stores texture Index for each kind of texture
			This information is used on GPU side
		*/

		u32 flags = 0;
		u32 textureIds[6] = { 0 };
		u8 diffuseTextureCount = 0; // We can have two Diffuse textures for a material
		for (int i = 0; i < mw.m_material->TextureNameType.size(); i++)
		{
			int txtID = -1;
			switch (mw.m_material->TextureNameType[i].first)
			{
			case TEXTURE_TYPE::TEXTURE_TYPE_DIFFUSE: txtID = diffuseTextureCount++; break;
			case TEXTURE_TYPE::TEXTURE_TYPE_NORMAL: txtID = 2;  break;
			case TEXTURE_TYPE::TEXTURE_TYPE_SPECULAR: txtID = 3;  break;
			case TEXTURE_TYPE::TEXTURE_TYPE_TRANSPARENCY: txtID = 4; break;
			}

			if (diffuseTextureCount > 1) diffuseTextureCount = 1; // we support only two Diffuse Textures

			if (txtID >= 0)
			{
				flags |= (1 << txtID);
				textureIds[txtID] = mw.m_loader->textureIDByName(mw.m_material->TextureNameType[i].second);
			}
		}

		file.write(reinterpret_cast<char*>(&flags), sizeof flags);
		file.write(reinterpret_cast<char*>(textureIds), sizeof textureIds);
	}

	//Roughness
	{
		auto value = mw.m_material->Roughness;
		file.write(reinterpret_cast<char*>(&value), sizeof(value));
	}

	// Flags
	{
		char flags[5] =
		{
			mw.m_material->isTransparencyUsed,
			mw.m_material->isTransparent,
			mw.m_material->isSky,
			mw.m_material->doesIncludeToWorldBB,
			mw.m_material->doesItHaveNormalTexture
		};

		file.write(flags, sizeof flags);
	}
}