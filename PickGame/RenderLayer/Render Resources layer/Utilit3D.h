#pragma once
#ifndef UTILIT3D_H
#define UTILIT3D_H
#include <comdef.h>
#include <assert.h>
#include <string>
#include "../d3dx12.h"
#include "../../Plugins/TextureLoader/DDSTextureLoader.h"
#include "../../Plugins/TextureLoader/DirectXHelpers.h"
#include <D3DCompiler.h>

class Utilit3D
{
	static ID3D12Device* m_device;
	static ID3D12GraphicsCommandList* m_cmdList;
	static bool m_initialized;
public:
	Utilit3D();
	~Utilit3D();
	
	static void initialize(ID3D12Device* iDevice, ID3D12GraphicsCommandList* iCmdList);
	static ID3D12Device* getDevice() { assert(m_initialized); return m_device; }
	static ID3D12GraphicsCommandList* getCmdList() { assert(m_initialized); return m_cmdList; }

	// ======================================================  Non-static versions

	//Upload mesh data to Default Heap Buffer
	/*use tamplate here to avoid of any using external data structures*/
	//template<typename Mesh, typename VertexGPU, typename IndexGPU>
	//void UploadMeshToDefaultBuffer(Mesh* mesh, std::vector<VertexGPU> vertexData, std::vector<IndexGPU> indexData);

	// ======================================================  Static versions

	static UINT CalcConstantBufferByteSize (UINT byteSize);

	static winrt::com_ptr<ID3D12Resource> createDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 byteSize,
		winrt::com_ptr<ID3D12Resource>& uploadBuffer);

	static winrt::com_ptr<ID3D12Resource> createDefaultBuffer(
		const void* initData,
		UINT64 byteSize,
		winrt::com_ptr<ID3D12Resource>& uploadBuffer);

	static winrt::com_ptr<ID3D12Resource> createTextureWithData(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 elementByteSize,
		UINT Width,
		UINT Height,
		DXGI_FORMAT textureFormat,
		winrt::com_ptr<ID3D12Resource>& uploadBuffer,
		D3D12_RESOURCE_FLAGS textureFlags = D3D12_RESOURCE_FLAG_NONE);

	// for wstring file name
	static ID3DBlob* compileShader(
		const std::wstring& filename,
		const D3D_SHADER_MACRO* defines,
		const std::string& entrypoint,
		const std::string& target);

	// for string file name
	static ID3DBlob* compileShader(
		const std::string& filename,
		const D3D_SHADER_MACRO* defines,
		const std::string& entrypoint,
		const std::string& target);

	// for wstring file name
	static bool compileShader(
		const std::wstring& filename,
		const D3D_SHADER_MACRO* defines,
		const std::string& entrypoint,
		const std::string& target,
		ID3D10Blob** ptrBlob);

	// for string file name
	static bool compileShader(
		const std::string& filename,
		const D3D_SHADER_MACRO* defines,
		const std::string& entrypoint,
		const std::string& target,
		ID3D10Blob** ptrBlob);

	static void UploadDDSTexture(
		std::string textureFileName,
		winrt::com_ptr<ID3D12Resource> &textureResource,
		winrt::com_ptr<ID3D12Resource> &uploader);

	//Upload mesh data to Default Heap Buffer
	/*use tamplate here to avoid of any using external data structures*/
	template<typename Mesh, typename VertexGPU, typename IndexGPU>
	static void UploadMeshToDefaultBuffer(Mesh* mesh, std::vector<VertexGPU> vertexData, std::vector<IndexGPU> indexData);
};

// template methods defenitions
template<typename Mesh, typename VertexGPU, typename IndexGPU>
void Utilit3D::UploadMeshToDefaultBuffer(
	Mesh* mesh, std::vector<VertexGPU> vertexData, std::vector<IndexGPU> indexData)
{
	assert(m_initialized && "Utilit3D is not initialized"); //should be initialized at first

	int vertexByteStride = sizeof(VertexGPU);

	mesh->VertexBufferByteSize =  sizeof(VertexGPU) * (UINT) vertexData.size();
	mesh->IndexBufferByteSize =  sizeof(IndexGPU) * (UINT) indexData.size();
	mesh->VertexBufferStride = vertexByteStride;
	mesh->IndexFormat = DXGI_FORMAT_R32_UINT;

	mesh->VertexGPUBuffer.ResourceInDefaultHeap = Utilit3D::createDefaultBuffer(m_device, m_cmdList,
		vertexData.data(),
		mesh->VertexBufferByteSize,
		mesh->VertexGPUBuffer.ResourceInUploadHeap);

	mesh->IndexGPUBuffer.ResourceInDefaultHeap = Utilit3D::createDefaultBuffer(m_device, m_cmdList,
		indexData.data(),
		mesh->IndexBufferByteSize,
		mesh->IndexGPUBuffer.ResourceInUploadHeap);

	// Store the same data for CPU using
	D3DCreateBlob(mesh->VertexBufferByteSize, mesh->VertexCPUBuffer.put());
	assert(mesh->VertexCPUBuffer);
	CopyMemory(mesh->VertexCPUBuffer->GetBufferPointer(), vertexData.data(), mesh->VertexBufferByteSize);

	D3DCreateBlob(mesh->IndexBufferByteSize, mesh->IndexCPUBuffer.put());
	assert(mesh->IndexCPUBuffer);
	CopyMemory(mesh->IndexCPUBuffer->GetBufferPointer(), indexData.data(), mesh->IndexBufferByteSize);
}

//template<typename Mesh, typename VertexGPU, typename IndexGPU>
//void Utilit3D::UploadMeshToDefaultBuffer(Mesh* mesh, std::vector<VertexGPU> vertexData, std::vector<IndexGPU> indexData)
//{
//	if (m_initialized)
//		Utilit3D::UploadMeshToDefaultBuffer<Mesh, VertexGPU, IndexGPU>(mesh, vertexData, indexData, m_device, m_cmdList);
//	else
//		assert(0);
//}
#endif UTILIT3D_H