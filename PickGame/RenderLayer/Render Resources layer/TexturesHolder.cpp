#include "TexturesHolder.h"
#include "Utilit3D.h"
#include "DXGILayer.h"

#include <cassert>

using namespace std;

void TexturePack::buildTextureSRVs_()
{
	// 1 - Create SRV Descriptor Heap
	{
		// At least one SRV we need to have
		if (m_textures.size() == 0)
			m_textures.push_back(ResourceWithUploader());

		m_SVRCount = m_textures.size();

		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = m_SVRCount;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE; // No need this Views be accessible for GPU, we will copy these view to another SRV heap later
		srvHeapDesc.NodeMask = 0;

		Utilit3D::getDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(m_SRVHeap.put()));
		DirectX::SetDebugObjectName(m_SRVHeap.get(), "TextureSRVHeap_pack1");
	}

	// 2 - Prepare some values
	u32 srvSize = Utilit3D::getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_SRVHeap->GetCPUDescriptorHandleForHeapStart());

	// 3 - Create SRV for texture resources
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		for (auto& texture : m_textures)
		{
			if (texture.ResourceInDefaultHeap != NULL)
			{
				// Create SRV for real Texture
				srvDesc.Format = texture.ResourceInDefaultHeap->GetDesc().Format;
				srvDesc.Texture2D.MipLevels = texture.ResourceInDefaultHeap->GetDesc().MipLevels;

				Utilit3D::getDevice()->CreateShaderResourceView(texture.ResourceInDefaultHeap.get(), &srvDesc, handle);
			}
			else
			{
				// Create empty SRV
				srvDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
				srvDesc.Texture2D.MipLevels = 1;

				Utilit3D::getDevice()->CreateShaderResourceView(nullptr, &srvDesc, handle);
			}

			handle.Offset(1, srvSize);
		}
	}
}

void TexturePack::loadTexturesToGPU()
{
	assert(!m_textureWereUploadedToGPU);

	m_textures.resize(m_orderedTextureNames.size());

	for (u32 i = 0; i < m_orderedTextureNames.size(); i++)
	{
		Utilit3D::UploadDDSTexture(
			m_orderedTextureNames[i],
			m_textures[i].ResourceInDefaultHeap,
			m_textures[i].ResourceInUploadHeap);
	}

	m_textureWereUploadedToGPU = true;

	buildTextureSRVs_();
}

// --------------------------------------- --------------------------------------------------
// --------------------------------------- TexturesHolder -----------------------------------
// ------------------------------------------------------ -----------------------------------

TexturesHolder::TexturesHolder():
	_nextPackId(0)
{
}

TexturesHolder::~TexturesHolder()
{
	for (auto& pack : m_packs)
		delete pack.second;
}

void TexturesHolder::initialize(DXGILayer* dxgiLayer)
{
	assert(dxgiLayer);

	m_dxgiLayer = dxgiLayer;
}

u16 TexturesHolder::newPack()
{
	// Just to be sure  that we do not have the pack with id = _nextPackId;
	assert(!getPack(_nextPackId));

	m_packs[_nextPackId++] = new TexturePack();

	return _nextPackId - 1;
}

TexturePack* TexturesHolder::getPack(u16 packId)
{
	auto it = m_packs.find(packId);
	if (it != m_packs.end())
		return it->second;
	else
		return nullptr;
}

void TexturesHolder::freePack(u16 packId)
{
	m_packs.erase(packId);
}

void TexturesHolder::joinSRVs(u16 packId1, u16 packId2)
{
	auto pack1 = getPack(packId1);
	auto pack2 = getPack(packId2);

	assert(m_dxgiLayer);
	assert(pack1 || pack2);
	

	// 0 - Clear SRV Heap if it is used
	{
		if (m_SRVHeap.get())
		{
			// wait...
			m_dxgiLayer->flushCommandQueue();
			
			// now!
			m_SRVHeap = nullptr;
		}
	}

	// 1 - Create SRV Descriptor Heap
	{
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = pack1 ? pack1->getSRVCount() : 0 + pack2 ? pack2->getSRVCount() : 0;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		srvHeapDesc.NodeMask = 0;

		Utilit3D::getDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(m_SRVHeap.put()));
		DirectX::SetDebugObjectName(m_SRVHeap.get(), "TextureSRVHeapCommon");
	}

	// 2 - Copy SRVs for Pack 1
	{
		if (pack1)
		{
			Utilit3D::getDevice()->CopyDescriptorsSimple(
				pack1->getSRVCount(),
				m_SRVHeap->GetCPUDescriptorHandleForHeapStart(),
				pack1->getSRVHeap()->GetCPUDescriptorHandleForHeapStart(),
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}
	}

	// 3 - Copy SRVs for Pack 2
	{
		if (pack2)
		{
			auto cpuHandle = m_SRVHeap->GetCPUDescriptorHandleForHeapStart();
			auto descriptorSize = Utilit3D::getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			cpuHandle.ptr += pack1 ? descriptorSize * pack1->getSRVCount() : 0;

			Utilit3D::getDevice()->CopyDescriptorsSimple(
				pack2->getSRVCount(),
				cpuHandle,
				pack2->getSRVHeap()->GetCPUDescriptorHandleForHeapStart(),
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}
	}
}