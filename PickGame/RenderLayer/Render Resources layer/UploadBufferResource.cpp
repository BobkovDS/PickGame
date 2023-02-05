#include "UploadBufferResource.h"
#include "Utilit3D.h"

UploadBufferResource::UploadBufferResource():
	m_mappedData(nullptr),
	m_elementCount(0),
	m_elementByteSize(0)
{
}

void UploadBufferResource::_Create(u32 elementCount, u32 elementByteSize)
{
	assert(m_elementCount);
	assert(elementCount);

	m_elementCount = elementCount;
	m_elementByteSize = elementByteSize;

	auto res = Utilit3D::getDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(m_elementByteSize * m_elementCount),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_bufferResource.put()));

	assert(SUCCEEDED(res));

	res = m_bufferResource->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedData));
	assert(SUCCEEDED(res));

	/*	Check this variant: !!!!!
		CD3DX12_RANGE readRange(0, 0); // we do not need to read from this resource on CPU side
		res = mUploadBuffer->Map(0, &readRange, reinterpret_cast<void**>(&mMappedData));
	*/
}

UploadBufferResource::UploadBufferResource(u32 elementByteSize, u32 elementCount, bool isConstantBuffer):
	m_mappedData(nullptr)
{
	create(elementByteSize, elementCount, isConstantBuffer);
}

UploadBufferResource::~UploadBufferResource()
{
	if (m_bufferResource != nullptr)
		m_bufferResource->Unmap(0, nullptr);

	m_bufferResource = nullptr;
}

void UploadBufferResource::create(u32 elementByteSize, u32 elementCount, bool isConstantBuffer)
{
	assert(elementByteSize);
	assert(elementCount);

	_Create(elementCount, isConstantBuffer ? Utilit3D::CalcConstantBufferByteSize(elementByteSize) : elementByteSize);
}

ID3D12Resource* UploadBufferResource::getResource()
{
	if (m_bufferResource == nullptr) return nullptr;
	else
		return m_bufferResource.get();
}

void UploadBufferResource::copyData(u32 elementId, BYTE* data)
{
	assert(elementId < m_elementCount);

	memcpy(
		&m_mappedData[elementId * m_elementByteSize],
		data,
		m_elementByteSize);
}

void UploadBufferResource::copyData(u32 elementId, u32 count, BYTE* data)
{
	assert(elementId < m_elementCount);
	if (count <= m_elementCount)
		memcpy(
			&m_mappedData[elementId * m_elementByteSize],
			data,
			m_elementByteSize * count);
	else
	{
		m_bufferResource->Unmap(0, nullptr);
		m_bufferResource = nullptr;

		_Create(count, m_elementByteSize);

		memcpy(
			&m_mappedData[elementId * m_elementByteSize],
			data,
			m_elementByteSize * count);
	}
}

void UploadBufferResource::resize()
{
	
}

u32 UploadBufferResource::size()
{
	return m_elementCount;
}