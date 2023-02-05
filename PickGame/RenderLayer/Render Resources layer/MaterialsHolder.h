#pragma once
#include "gpuResources_materails_and_textures.h"
#include <map>
#include <vector>

class MaterialCPU;

class MaterialPack
{
	std::vector<MaterialCPU*> m_materials;
	ResourceWithUploader m_gpuMaterialsResource;
	bool m_materialsAlreadyUploadedToGPU; // it means only that it was a command to upload materials to GPU

public:
	MaterialPack() :
		m_materialsAlreadyUploadedToGPU(false)
	{
		m_materials.clear();
	}

	MaterialCPU* createMaterial();
	void loadMaterialsToGPU();
	void freeUploadResource() {};

	ID3D12Resource* getResource() { return m_gpuMaterialsResource.ResourceInDefaultHeap.get(); }
};

class MaterialsHolder
{
	std::map<u16, MaterialPack*> m_packs;
	u16 m_packsCount;

public:
	MaterialsHolder();
	~MaterialsHolder();

	u16 newPack();
	MaterialPack* getPack(u16 packId);
	void freePack(u16 packId);
};