#include "PGFFileLoader.h"

#include "../../Others/Logger.h"

using namespace std;


struct MeshReader
{
	PGFFileLoader::Mesh* m_mesh;

	MeshReader(PGFFileLoader::Mesh* mesh)
	{
		m_mesh = mesh;
	}
	friend void operator>>(std::ifstream& file, MeshReader& mr);
};

struct MaterialReader
{
	PGFFileLoader::Material* _material;

	MaterialReader(PGFFileLoader::Material* m)
	{
		_material = m;
	}
	friend void operator>>(std::ifstream& file, MaterialReader& mr);
};

struct InstanceReader
{
	PGFFileLoader::Instance* _instance;

	InstanceReader(PGFFileLoader::Instance* m)
	{
		_instance = m;
	}
	friend void operator>>(std::ifstream& file, InstanceReader& ir);
};

struct ReadBuffer
{
	u32 _size;

public:
	char* buffer;
	ReadBuffer(u32 size): _size(size)
	{
		buffer = new char[_size];
		memset(buffer, 0, _size);
	}

	~ReadBuffer()
	{
		delete[_size] buffer;
	}

	void operator>>(void* dst)
	{
		assert(dst);
		memcpy(dst, buffer, _size);
	}
	
	template<typename T>
	void operator>>(std::vector<T>& v)
	{
		assert((v.size() * sizeof(T)) == _size);

		void* ptr = reinterpret_cast<void*>(&v[0]);
		memcpy(ptr, buffer, _size);
	}
};

template<typename T>
void readVector(std::ifstream& file, std::vector<T>& v)
{
	union reader
	{
		u32 count;
		char buffer[4];
	} r;

	// Read element count in vector
	file.read(r.buffer, 4);
	v.resize(r.count);

	if (r.count == 0)
		return;

	// Read buffer size in bytes
	file.read(r.buffer, 4);
	
	// Read this buffer to vector
	ReadBuffer b(r.count);
	file.read(b.buffer, r.count);
	b >> v;
}

void readString(std::ifstream& file, std::string& s)
{
	union reader
	{
		u32 size;
		char buffer[4];
	} r;
	
	file.read(r.buffer, 4);

	if (r.size == 0)
		return;
	
	ReadBuffer b(r.size + 1);
	file.read(b.buffer, r.size);

	s = std::string(b.buffer);
}

void PGFFileLoader::readMeshData()
{
	char dataID[11] = { 0 };
	m_modelFile.read(dataID, 10);

	string sDataID(dataID);
	assert(sDataID == "MESH******");
	

	union reader
	{
		u32 count;
		char buffer[4];
	} r;

	m_modelFile.read(r.buffer, 4);

	u32 meshCount = r.count;
	m_meshes.resize(meshCount);
	
	Logger::logln("Reading Meshes ["+ std::to_string(meshCount) +"]");

	for (auto& mesh : m_meshes)
	{
		m_modelFile >> MeshReader(&mesh);
	}
}

void PGFFileLoader::readMaterialData()
{
	char dataID[11] = { 0 };
	m_modelFile.read(dataID, 10);

	string sDataID(dataID);
	assert(sDataID == "MATERIALS*");

	union reader
	{
		u32 count;
		char buffer[4];
	} r;

	m_modelFile.read(r.buffer, 4);

	u32 materialCount = r.count;
	m_materials.resize(materialCount);

	Logger::logln("Reading Materials [" + std::to_string(materialCount) + "]");

	for (auto& material : m_materials)
	{
		m_modelFile >> MaterialReader(&material);
	}
}

void PGFFileLoader::readInstancesData()
{
	char dataID[11] = { 0 };
	m_modelFile.read(dataID, 10);

	string sDataID(dataID);
	assert(sDataID == "INSTANCES*");

	union reader
	{
		u32 count;
		char buffer[4];
	} r;

	m_modelFile.read(r.buffer, 4);

	u32 instancesCount = r.count;

	Logger::logln("Reading Instances [" + std::to_string(instancesCount) + "]");

	for (u32 i = 0; i < instancesCount; i++)
	{
		Instance instance;
		m_modelFile >> InstanceReader(&instance);
		m_instancesByMeshId[instance.MeshID].push_back(instance);
	}
}

void PGFFileLoader::readTextureData()
{
	char dataID[11] = { 0 };
	m_modelFile.read(dataID, 10);

	string sDataID(dataID);
	assert(sDataID == "TEXTURES**");

	union reader
	{
		u32 count;
		char buffer[4];
	} r;

	m_modelFile.read(r.buffer, 4);

	u32 textureCount = r.count;
	m_textures.resize(textureCount);

	Logger::logln("Reading Textures [" + std::to_string(textureCount) + "]");

	for (auto& texture: m_textures)
		readString(m_modelFile, texture);

}

void operator>>(std::ifstream& file, MeshReader& mr)
{
	/*
	Mesh_name_buffer_size		[in bytes]
	Mesh_name_buffer
	
	Mesh_indices_count			[4 bytes]
	Mesh_indices_buffer_Size	[in bytes]
	Mesh_indices_buffer

	Mesh_vertices_count			[4 bytes]
	Mesh_vertices_buffer_Size	[in bytes]
	Mesh_vertices_buffer

	std::vector<DirectX::XMFLOAT3> Normals;
	std::vector<DirectX::XMFLOAT3> Tangents;
	std::vector<DirectX::XMFLOAT3> BiTangents;
	std::vector<DirectX::XMFLOAT2> UVs;

	byte ExcludeFromCulling;			[char]
	byte ExcludeFromMirrorReflection;	[char]
	byte DoNotDublicateVertices;		[char]
	byte MaterialHasNormalTexture;		[char]
	*/
	union reader
	{
		u32 count;
		char buffer[4];
	} r;

	// Mesh Name size
	file.read(r.buffer, 4);

	// Mesh Name value
	ReadBuffer meshName(r.count + 1);

	file.read(meshName.buffer, r.count);
	string smn(meshName.buffer);

	// Mesh Indices data
	readVector(file, mr.m_mesh->Indices);

	// Mesh Vertices data
	readVector(file, mr.m_mesh->Vertices);

	// Read Normals data
	readVector(file, mr.m_mesh->Normals);

	// Read Tangent data
	readVector(file, mr.m_mesh->Tangents);

	// Read BiTangents data
	readVector(file, mr.m_mesh->BiTangents);

	// Read UVs data
	readVector(file, mr.m_mesh->UVs);

	// Read Flags
	{
		ReadBuffer b(4);
		file.read(b.buffer, 4);
		
		mr.m_mesh->ExcludeFromCulling =			(b.buffer[0] != 0);
		mr.m_mesh->ExcludeFromMirrorReflection= (b.buffer[1] != 0);
		mr.m_mesh->DoNotDublicateVertices =		(b.buffer[2] != 0);
		mr.m_mesh->MaterialHasNormalTexture =	(b.buffer[3] != 0);
	}
}

void operator>>(std::ifstream& file, MaterialReader& mr)
{
	/*
		DirectX::XMFLOAT4 DiffuseAlbedo;
		DirectX::XMFLOAT3 FresnelR0;
		DirectX::XMFLOAT4X4 MaterialTransformation;

		Material_texture_flags	[4 bytes]
		Material_texture_IDs	[24 bytes] (6x4 bytes)

		float Roughness;				[4 bytes]
		bool isTransparencyUsed;		[char]
		bool isTransparent;				[char]
		bool isSky;						[char]
		bool doesIncludeToWorldBB;		[char]
		bool doesItHaveNormalTexture;	[char]
	*/

	union reader
	{
		u32 count;
		char buffer[4];
	} r;

	// DiffuseAlbedo
	file.read(reinterpret_cast<char*>(&mr._material->DiffuseAlbedo), sizeof(mr._material->DiffuseAlbedo));

	// FresnelR0
	file.read(reinterpret_cast<char*>(&mr._material->FresnelR0), sizeof(mr._material->FresnelR0));

	// MaterialTransformation
	file.read(reinterpret_cast<char*>(&mr._material->MaterialTransformation), sizeof(mr._material->MaterialTransformation));

	// Texture Flags
	file.read(reinterpret_cast<char*>(&mr._material->Flags), sizeof(mr._material->Flags));

	// Texture Map IDs
	file.read(reinterpret_cast<char*>(&mr._material->TextureMapIDs), sizeof(mr._material->TextureMapIDs));

	//Roughness
	file.read(reinterpret_cast<char*>(&mr._material->Roughness), sizeof(mr._material->Roughness));

	// Read Flags
	{
		char b[5];
		file.read(b, sizeof b);

		mr._material->isTransparencyUsed		= b[0] != 0;
		mr._material->isTransparent				= b[1] != 0;
		mr._material->isSky						= b[2] != 0;
		mr._material->doesIncludeToWorldBB		= b[3] != 0;
		mr._material->doesItHaveNormalTexture	= b[4] != 0;
	}
}

void operator>>(std::ifstream& file, InstanceReader& ir)
{
	/*
		MeshID [4 bytes]
		bool Visible; [char]

		Intances_materials_count
		{
			Intances_materials_ID [4 bytes]
		}

		DirectX::XMFLOAT3 Transformation;
		DirectX::XMFLOAT3 Translation;
		DirectX::XMFLOAT4X4 GlobalTransformation;
	*/

	union reader
	{
		u32 count;
		char buffer[4];
	} r;

	// MeshID
	file.read(reinterpret_cast<char*>(&ir._instance->MeshID), sizeof(ir._instance->MeshID));

	// Visible
	char visible;
	file.read(&visible, 1);
	ir._instance->Visible = visible != 0;

	// Materials ID count
	file.read(r.buffer, 4);
	ir._instance->MaterialIDs.resize(r.count);

	for (auto& materialID : ir._instance->MaterialIDs)
	{
		file.read(r.buffer, 4);
		materialID = r.count;
	}

	// Transformation
	file.read(reinterpret_cast<char*>(&ir._instance->Transformation), sizeof(ir._instance->Transformation));

	// Translation
	file.read(reinterpret_cast<char*>(&ir._instance->Translation), sizeof(ir._instance->Translation));

	// GlobalTransformation
	file.read(reinterpret_cast<char*>(&ir._instance->GlobalTransformation), sizeof(ir._instance->GlobalTransformation));
}

void PGFFileLoader::loadFile(const std::string& fileName)
{
	Logger::logln("Reading model file: " + fileName);

	m_modelFile.open(fileName, ios_base::binary);

	if (m_modelFile.is_open())
	{
		readMeshData();
		readTextureData();
		readMaterialData();
		readInstancesData();
	}
	else
	{
		Logger::logln("File: " + fileName + " is not open");
	}
}

void PGFFileLoader::prepare()
{
	/* do some preparations, if it is required*/
}

u32 PGFFileLoader::instancesCount(u32 meshId) const
{
	auto it = m_instancesByMeshId.find(meshId);
	if (it != m_instancesByMeshId.end())
		return it->second.size();

	return 0;
}
