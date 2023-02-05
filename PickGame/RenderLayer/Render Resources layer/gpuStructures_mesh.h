#pragma once
#include "gpuResources_materails_and_textures.h"
#include <string>

#define LOD_COUNT 3

struct VertexGPU // Simple vertex structur
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT4 TangentU;
	DirectX::XMFLOAT2 UVText;
};

struct SubMeshInfo
{
	u32 IndexCount;
	u32 VertexCount;
	u32 StartIndexCount;
	u32 BaseVertexLocation;
};

struct InstanceData
{
	DirectX::XMFLOAT4X4 WorldMatrix;
	u32 MaterialId;

	InstanceData()
	{
		DirectX::XMStoreFloat4x4(&WorldMatrix, DirectX::XMMatrixIdentity());
		MaterialId = 0;
	}
};

struct Mesh
{
	std::string Name;
	u32 TrianglesCount;
	bool IsSkinnedData;
	u32 VertexBufferStride;
	u32 VertexBufferByteSize;
	u32 IndexBufferByteSize;
	DXGI_FORMAT IndexFormat;
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType;
	SubMeshInfo SubMesh;
	ResourceWithUploader VertexGPUBuffer;
	ResourceWithUploader IndexGPUBuffer;
	winrt::com_ptr<ID3DBlob> VertexCPUBuffer;
	winrt::com_ptr<ID3DBlob> IndexCPUBuffer;
	//BoundingBox;

	const D3D12_VERTEX_BUFFER_VIEW* getVertexGPUBufferView()
	{
		if (m_vertexBufferView.BufferLocation == 0)
		{
			m_vertexBufferView.BufferLocation = VertexGPUBuffer.ResourceInDefaultHeap->GetGPUVirtualAddress();
			m_vertexBufferView.StrideInBytes = VertexBufferStride;
			m_vertexBufferView.SizeInBytes = VertexBufferByteSize;
		}
			
		return &m_vertexBufferView;
	};

	const D3D12_INDEX_BUFFER_VIEW* getIndexGPUBuffer()
	{
		if (m_indexBufferView.BufferLocation == 0)
		{
			m_indexBufferView.BufferLocation = IndexGPUBuffer.ResourceInDefaultHeap->GetGPUVirtualAddress();
			m_indexBufferView.Format = IndexFormat;
			m_indexBufferView.SizeInBytes = IndexBufferByteSize;
		}

		return &m_indexBufferView;
	};

	Mesh()
	{
		m_vertexBufferView.BufferLocation = 0;
		m_indexBufferView.BufferLocation = 0;
	}

private:
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
};