#include "ResourceManager.h"
#include "DXGILayer.h"

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

void ResourceManager::initialize(DXGILayer* dxgiLayer)
{
	assert(dxgiLayer);
	m_dxgiLayer = dxgiLayer;

	DXGILayerInformation dxgiData = {};
	m_dxgiLayer->provideDXGIInformation(dxgiData);

	m_device = dxgiData.pDevice;
	m_pCmdQueue = dxgiData.pCmdQueue;
	
	m_cmdFence.create(m_device, m_pCmdQueue, 1, L"Resource Mngr");
	m_frameResourceManager.initialize(m_device, &m_cmdFence);
}

void ResourceManager::resetWorkCmdList()
{
	assert(m_dxgiLayer);
	m_dxgiLayer->resetCommandList();
}

void ResourceManager::executeWorkCmdList()
{
	assert(m_dxgiLayer);
	m_dxgiLayer->executeCommandList();
}