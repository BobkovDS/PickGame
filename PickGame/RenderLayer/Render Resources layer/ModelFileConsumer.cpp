#include "../../Include/ModelFileLoader.h"
#include "../../../FBXLoader/StringIDs.h"

#include "cpuResources.h"
#include "ModelFileConsumer.h"
#include "PGFFileLoader.h"

#include "SkinObject.h"
#include "ResourceManager.h"
#include "Utilit3D.h"

ModelFileConsumer::ModelFileConsumer(ResourceManager* resourceManager, SkinObject* newSkin)
{
	assert(resourceManager);
	assert(newSkin);

	_resourceManager = resourceManager;
	_newSkin = newSkin;
}

SceneObject* ModelFileConsumer::buildSceneObject(u32 meshIndex)
{
	using namespace DirectX;

	SceneObject* so = new SceneObject();

	auto mesh = _loader->mesh(meshIndex);
	auto soMesh = so->createMesh();
	
	soMesh->Name = "Mesh_" + std::to_string(meshIndex);
	soMesh->IsSkinnedData = mesh->VertexWeightByBoneName.size() > 0;

	so->addTag(SKIN_LEFT_PART); // TODO: Do this right
	so->setType(SceneObjectType::SOT_Opaque); // TODO: Do this right

	// Build mesh data with Vertices duplication

	std::vector<VertexGPU> vertices;
	std::vector<u32> indices;
	VertexGPU vertex;

	indices.resize(mesh->Indices.size());
	vertices.resize(mesh->Indices.size());

	bool doesHaveNormal = mesh->Normals.size();
	bool doesHaveUV = mesh->UVs.size();
	bool doesHaveTangent = mesh->Tangents.size();

	for (u32 i =0; i< indices.size(); i++)
	{
		vertex = {};
		u32 index = mesh->Indices[i];
		vertex.Pos = mesh->Vertices[index];
		
		if (doesHaveUV)
			vertex.UVText = mesh->UVs[i];

		if (doesHaveNormal)
			vertex.Normal = mesh->Normals[i];
		
		if (doesHaveTangent)
		{
			XMVECTOR normal = XMLoadFloat3(&mesh->Normals[i]);
			XMVECTOR tangent = XMLoadFloat3(&mesh->Tangents[i]);
			XMVECTOR biTangent = XMLoadFloat3(&mesh->BiTangents[i]);

			float w = XMVectorGetX(XMVector3Dot(XMVector3Cross(tangent, biTangent), normal));
			w = w > 0 ? 1.0f : -1.0f;
			XMStoreFloat4(&vertex.TangentU, tangent);
			vertex.TangentU.w = w;
		}

		vertices[i] = vertex;
		indices[i] = i;
	}

	soMesh->SubMesh = {};
	soMesh->SubMesh.VertexCount = u32(vertices.size());
	soMesh->SubMesh.IndexCount = u32(indices.size());
	soMesh->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	Utilit3D::UploadMeshToDefaultBuffer(soMesh, vertices, indices);

	auto instances = _loader->instancesForMesh(meshIndex);
	for (auto& inInstance : instances)
	{
		InstanceData outInstance;
		if (inInstance.MaterialIDs.size())
			outInstance.MaterialId = inInstance.MaterialIDs[0];

		outInstance.WorldMatrix = inInstance.GlobalTransformation;

		so->addInstance(outInstance);
	}
	return so;
}

void ModelFileConsumer::writeMaterials()
{
	auto materialCount = _loader->materialsCount();

	auto packID = _resourceManager->getMaterialsHolder()->newPack();
	auto materialPack = _resourceManager->getMaterialsHolder()->getPack(packID);	
	assert(materialPack);

	_newSkin->setMaterialPackId(packID); // Store which Material Pack ID is for this skin

	for (u32 i = 0; i < materialCount; i++)
	{
		auto inMaterial = _loader->material(i);
		auto outMaterial = materialPack->createMaterial();

		outMaterial->DiffuseAlbedo = inMaterial->DiffuseAlbedo;
		outMaterial->FresnelR0 = inMaterial->FresnelR0;
		outMaterial->Roughness = inMaterial->Roughness;
		outMaterial->MaterialTransformation = inMaterial->MaterialTransformation;
		outMaterial->Flags = inMaterial->Flags;
		memcpy(outMaterial->TextureMapIds, inMaterial->TextureMapIDs, sizeof outMaterial->TextureMapIds);
		outMaterial->doesIncludeToWorldBB = inMaterial->doesIncludeToWorldBB;
		outMaterial->doesItHaveNormalTexture = inMaterial->doesItHaveNormalTexture;
		outMaterial->isSky = inMaterial->isSky;
		outMaterial->isTransparencyUsed = inMaterial->isTransparencyUsed;
		outMaterial->isTransparent = inMaterial->isTransparent;
		
		outMaterial->MatCBIndex = inMaterial->MatCBIndex;
	}

	materialPack->loadMaterialsToGPU();
}

void ModelFileConsumer::consume(ModelFileLoader* fileLoader)
{
	// Write to ResourceManager
	_loader = dynamic_cast<PGFFileLoader*>(fileLoader);
	assert(_loader);

	_loader->prepare();
}

bool ModelFileConsumer::writeToDevice()
{
	//Write to Skin

	// Write Textures
	std::map<u32, u32> textureGPUIDById;
	textureGPUIDById.clear();
	auto packId = _resourceManager->getTextureHolder()->newPack();
	auto texturePack = _resourceManager->getTextureHolder()->getPack(packId);
	assert(texturePack);

	_newSkin->setTexturePackId(packId); // Store which Texture Pack ID is for this skin

	for (u32 i = 0; i < _loader->texturesCount(); i++)
	{
		auto texturePath = _loader->texture(i);
		textureGPUIDById[i] = texturePack->addTexturePath(texturePath);
	}

	texturePack->buildTextureFileNameList();
	texturePack->loadTexturesToGPU();

	auto& sceneObjectHolder = _newSkin->getObjectsHolder();
	// Build and Load Meshes
	
	for (u32 i=0; i<_loader->meshCount(); i++)
		sceneObjectHolder.addObject(buildSceneObject(i));

	// Write Materials
	writeMaterials();

	return true;
}