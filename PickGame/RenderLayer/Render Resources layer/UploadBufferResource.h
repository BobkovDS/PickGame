#pragma once
#include "gpuResources_materails_and_textures.h"

class UploadBufferResource
{
	winrt::com_ptr<ID3D12Resource> m_bufferResource;
	BYTE* m_mappedData;
	u32 m_elementCount;
	u32 m_elementByteSize;

	void _Create(u32 elementCount, u32 elementByteSize);

	COPY_FORBID(UploadBufferResource)
public:
	UploadBufferResource();
	UploadBufferResource(u32 elementByteSize, u32 elementCount, bool isConstantBuffer);
	~UploadBufferResource();
	void create(u32 elementByteSize, u32 elementCount, bool isConstantBuffer);
	ID3D12Resource* getResource();
	void copyData(u32 elementID, BYTE* data);
	void copyData(u32 elementID, u32 count, BYTE* data);
	void resize();
	u32 size();
};