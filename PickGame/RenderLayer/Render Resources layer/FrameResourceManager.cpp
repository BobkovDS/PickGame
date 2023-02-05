#include "FrameResourceManager.h"

FrameResourceManager::FrameResourceManager():
	m_currentFRId(0),
	m_initialized(false),
	m_pCmdFence(nullptr),
	m_newFrame(false)
{
}

FrameResourceManager::~FrameResourceManager()
{	
}

void FrameResourceManager::initialize(ID3D12Device* pDevice, CommandFence* pCmdFence)
{
	assert(pCmdFence != nullptr);

	m_pCmdFence = pCmdFence;

	for (u8 i = 0; i < g_FrameResourceCount; i++)
	{
		m_frameResources[i].initialize(pDevice);
	}
}

void FrameResourceManager::getFreeFR()
{
	assert(m_newFrame == false && "New Frame was already got");
	m_newFrame = true;

	// Update next Frame Id
	m_currentFRId = (m_currentFRId + 1) % g_FrameResourceCount;
	
	// Wait when Frame with this ID is free
	u64 fenceValueToWait = m_frameResources[m_currentFRId].getFenceValue();
	m_pCmdFence->waitForFenceValue(fenceValueToWait);
	
	// Set this Frame as current free Frame
	m_currentFR = &m_frameResources[m_currentFRId];
}

void FrameResourceManager::frameDone()
{
	m_currentFR->setFenceValue(m_pCmdFence->setFence());
	m_newFrame = false;
}

void FrameResourceManager::changeCmdAllocator(ID3D12GraphicsCommandList* cmdList, ID3D12PipelineState* pso)
{
	ID3D12CommandAllocator* currentCmdAllocator = m_currentFR->getCommandAllocator();
	
	HRESULT res = currentCmdAllocator->Reset();
	assert(SUCCEEDED(res));

	res = cmdList->Reset(currentCmdAllocator, pso);
	if (!SUCCEEDED(res))
	{
		int a = 10;
	}
	assert(SUCCEEDED(res));
}

UploadBufferResource* FrameResourceManager::getUploadBuffer(const std::string& bufferName)
{
	return m_currentFR->getUploadBuffer(bufferName);
}

UploadBufferResource* FrameResourceManager::getMainPassCB()
{
	return m_currentFR->getUploadBuffer("MainPassCB");
}

UploadBufferResource* FrameResourceManager::getInstancesCB()
{
	return m_currentFR->getUploadBuffer("InstancesCB");
}

void FrameResourceManager::addUploadBuffer(const std::string& bufferName, bool isConstant, u32 elementCount)
{

}

void FrameResourceManager::resizeUploadBuffer(const std::string& bufferName, u32 elementCount)
{

}