#pragma once
#include "gpuResources_materails_and_textures.h"
#include <map>
#include <string>
#include <vector>

class TexturePack
{
private:
	std::map<std::string, u32> m_texturePathToIDMapping;
	std::vector<std::string> m_orderedTextureNames;
	std::vector<ResourceWithUploader> m_textures;
	winrt::com_ptr<ID3D12DescriptorHeap> m_SRVHeap;
	u32 m_SVRCount;

	bool m_textureWereUploadedToGPU; // it means that it already was a command to upload a textures to GPU

	void buildTextureSRVs_();
public:

	TexturePack() :
		m_textureWereUploadedToGPU(false),
		m_SVRCount(0)
	{}

	u32 addTexturePath(const std::string& texturePath)
	{
		m_orderedTextureNames.push_back(texturePath);
		return m_orderedTextureNames.size() - 1;
	}

	void buildTextureFileNameList()
	{
		for (auto& txtPath : m_orderedTextureNames)
		{
			txtPath.replace(txtPath.size() - 3, 3, "DDS");
		}
	}

	void freeTextureUploadHeaps()
	{
		assert(m_textureWereUploadedToGPU);
	}

	void loadTexturesToGPU();

	u32 getSRVCount() { return m_SVRCount; }

	ID3D12DescriptorHeap* getSRVHeap()
	{
		assert(m_SRVHeap.get());
		return m_SRVHeap.get();
	}
};

class DXGILayer;

class TexturesHolder
{
	winrt::com_ptr<ID3D12DescriptorHeap> m_SRVHeap;
	std::map<u16, TexturePack*> m_packs;
	u16 m_packsCount;
	DXGILayer* m_dxgiLayer;

	COPY_FORBID(TexturesHolder)
public:
	TexturesHolder();
	~TexturesHolder();

	void initialize(DXGILayer* m_dxgiLayer);

	u16 newPack();
	TexturePack* getPack(u16 packId);
	void freePack(u16 packId);
	void joinSRVs(u16 packId1, u16 packId2);

	ID3D12DescriptorHeap* getSRVHeap()
	{
		assert(m_SRVHeap.get());
		return m_SRVHeap.get();
	}
};