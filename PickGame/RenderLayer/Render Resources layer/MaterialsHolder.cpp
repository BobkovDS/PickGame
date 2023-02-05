#include "MaterialsHolder.h"
#include "cpuResources.h"
#include "Utilit3D.h"

MaterialCPU* MaterialPack::createMaterial()
{
	auto newMaterial = new MaterialCPU();
	m_materials.push_back(newMaterial);
	return newMaterial;
}

void MaterialPack::loadMaterialsToGPU()
{
	assert(m_materialsAlreadyUploadedToGPU == false);

	std::vector<MaterialData> toGPUData;

	for (auto& cpuMat : m_materials)
	{
		MaterialData gpuMat = {};
		gpuMat.DiffuseAlbedo = cpuMat->DiffuseAlbedo;
		gpuMat.FresnelR0 = cpuMat->FresnelR0;
		gpuMat.Roughness = cpuMat->Roughness;
		gpuMat.MatTransformation = cpuMat->MaterialTransformation;
		gpuMat.Flags = cpuMat->Flags;

		for (u8 i = 0; i < 6; i++)
			gpuMat.TextureMapIndex[i] = cpuMat->TextureMapIds[i];

		toGPUData.push_back(gpuMat);
	}

	m_gpuMaterialsResource.ResourceInDefaultHeap =
		Utilit3D::createDefaultBuffer(
			toGPUData.data(),
			toGPUData.size() * sizeof(MaterialData),
			m_gpuMaterialsResource.ResourceInUploadHeap);

	m_materialsAlreadyUploadedToGPU = true;
}

// --------------------------------------- --------------------------------------------------
// --------------------------------------- MaterialsHolder ----------------------------------
// ------------------------------------------------------- ----------------------------------

MaterialsHolder::MaterialsHolder():
	m_packsCount(0)
{
}

MaterialsHolder::~MaterialsHolder()
{
	for (auto& pack: m_packs)
		delete pack.second;
}

u16 MaterialsHolder::newPack()
{
	auto it = m_packs.find(m_packsCount);
	assert(it == m_packs.end());

	m_packs[m_packsCount++] = new MaterialPack();

	return m_packsCount - 1;
}

MaterialPack* MaterialsHolder::getPack(u16 packId)
{
	auto it = m_packs.find(packId);
	if (it != m_packs.end())
		return it->second;
	else
		return nullptr;
}

void MaterialsHolder::freePack(u16 packId)
{
	m_packs.erase(packId);
}