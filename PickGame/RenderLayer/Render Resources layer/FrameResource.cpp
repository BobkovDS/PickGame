#include "FrameResource.h"
#include "gpuStructures_mesh.h"
#include <cassert>

FrameResource::FrameResource():
	m_fenceValue(0)
{
}

FrameResource::~FrameResource()
{
	auto begin_it = m_resources.begin();
	auto end_it = m_resources.end();

	for (; begin_it != end_it; ++begin_it)
		delete begin_it->second;

	m_resources.clear();
}

void FrameResource::initialize(ID3D12Device* pDevice)
{
	HRESULT res = pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_cmdAllocator.put()));
	assert(SUCCEEDED(res));
	m_resources.clear();

	addNonConstantBuffer("InstancesCB", sizeof(InstanceData), 1);
	addConstantBuffer("MainPassCB", sizeof(PassConstantsGPU), 1);
}

void FrameResource::addBuffer(const std::string& bufferName, u32 elementByteSize, u32 elementCount, bool isConstantBuffer)
{
	auto find_it = m_resources.find(bufferName);
	if (find_it == m_resources.end())
	{
		UploadBufferResource* newBuffer = new UploadBufferResource(elementByteSize, elementCount, isConstantBuffer);
		m_resources[bufferName] = newBuffer;
	}
}

void FrameResource::addBuffer(const BufferDescription& bufferDescription)
{
	addBuffer(
		bufferDescription.BufferName,
		bufferDescription.ElementByteSize,
		bufferDescription.ElementCount,
		bufferDescription.IsConstantBuffer);
}

void FrameResource::addNonConstantBuffer(const std::string& bufferName, u32 elementByteSize, u32 elementCount)
{	
	addBuffer(bufferName, elementByteSize, elementCount, false);
}

void FrameResource::addConstantBuffer(const std::string& bufferName, u32 elementByteSize, u32 elementCount)
{
	addBuffer(bufferName, elementByteSize, elementCount, true);
}

UploadBufferResource* FrameResource::getUploadBuffer(const std::string& bufferName)
{
	auto find_iterator = m_resources.find(bufferName);

	assert(find_iterator != m_resources.end() && "FrameResource Buffer not found");
	return find_iterator->second;
}

D3D12_GPU_VIRTUAL_ADDRESS FrameResource::getUploadBufferGPUAddress(const std::string& bufferName)
{
	UploadBufferResource* buffer = getUploadBuffer(bufferName);
	
	if (buffer)
		return buffer->getResource()->GetGPUVirtualAddress();
	else
		return NULL;
}