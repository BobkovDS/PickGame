#pragma once
#include "UploadBufferResource.h"
#include <string>
#include <map>

struct BufferDescription
{
	std::string BufferName;// name for buffer
	u32 ElementByteSize; // a size of each element in Buffer
	u32 ElementCount; // elements count in a buffer
	bool IsConstantBuffer; // Is it ConstantBuffer ? (or it os a Structure buffer)
};

class FrameResource
{
	std::map<std::string, UploadBufferResource*> m_resources;
	winrt::com_ptr<ID3D12CommandAllocator> m_cmdAllocator;
	u64 m_fenceValue;

	COPY_FORBID(FrameResource)
public:
	FrameResource();
	~FrameResource();
	void initialize(ID3D12Device* pDevice);
	void addBuffer(const std::string& bufferName, u32 elementByteSize, u32 elementCount, bool isConstantBuffer);
	void addBuffer(const BufferDescription& bufferDescription);
	void addNonConstantBuffer(const std::string& bufferName, u32 elementByteSize, u32 elementCount); // specific version for addBuffer()
	void addConstantBuffer(const std::string& bufferName, u32 elementByteSize, u32 elementCount); // specific version for addBuffer()

	UploadBufferResource* getUploadBuffer(const std::string& bufferName);
	D3D12_GPU_VIRTUAL_ADDRESS getUploadBufferGPUAddress(const std::string& bufferName);
	ID3D12CommandAllocator* getCommandAllocator();
	u64 getFenceValue();
	void setFenceValue(u64 value);
};

inline ID3D12CommandAllocator* FrameResource::getCommandAllocator()
{
	return m_cmdAllocator.get();
}

inline u64 FrameResource::getFenceValue()
{
	return m_fenceValue;
}

inline void FrameResource::setFenceValue(u64 value)
{
	m_fenceValue= value;
}