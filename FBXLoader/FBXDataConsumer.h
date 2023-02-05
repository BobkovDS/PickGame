#pragma once

#include <fstream>

#include "..\PickGame\Include\ModelFileLoader.h"

class FBXFileLoader;

class FBXDataConsumer: public ModelDataConsumer
{
	std::ofstream m_outFile;

	bool prepareOutputFile_();
	void writeMeshes_();
	void writeTextures();
	void writeInstances_();
	void writeMaterials_();

	FBXFileLoader* m_loader;

public:
	void consume(ModelFileLoader* fileLoader) override;
	bool writeToDevice() override;
};

/*
	DATAID [10 chars] {<MESHES> / <INSTANCES> / <TEXTURES> / <MATERIALS>}
		<MESHES>
	Meshes_count					[4 bytes]
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

	...
	<TEXTURES>
		Textures_Count						[4 bytes]
		{
			Texture_Path_buffer_size		[4 bytes, size in bytes]
			Texture_Path_buffer
		}

	...
	<MATERIALS>
		Materials_count				[4 bytes]

		DirectX::XMFLOAT4 DiffuseAlbedo; (DiffuseAlbedo.w is Transparency factor)
		DirectX::XMFLOAT3 FresnelR0;
		DirectX::XMFLOAT4X4 MaterialTransformation;

		Material_texture_flags	[4 bytes]
		Material_texture_IDs	[24 bytes] [6x4 bytes)

		float Roughness;				[4 bytes]
		bool isTransparencyUsed;		[char]
		bool isTransparent;				[char]
		bool isSky;						[char]
		bool doesIncludeToWorldBB;		[char]
		bool doesItHaveNormalTexture;	[char]

	...
	<INSTANCES>
	Instances_count [4 bytes]

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