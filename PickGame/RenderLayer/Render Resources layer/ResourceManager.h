#pragma once
#include "FrameResourceManager.h"
#include "MaterialsHolder.h"
#include "TexturesHolder.h"

class DXGILayer;

class ResourceManager
{
	DXGILayer* m_dxgiLayer;
	FrameResourceManager m_frameResourceManager;
	MaterialsHolder m_materialsHolder;
	TexturesHolder m_texturesHolder;
	CommandFence m_cmdFence;

	ID3D12Device* m_device;
	ID3D12CommandQueue* m_pCmdQueue;

public:
	ResourceManager();
	~ResourceManager();
	void initialize(DXGILayer* dxgiLayer);
	FrameResourceManager* getFrameResourceManager() { return &m_frameResourceManager; }
	MaterialsHolder* getMaterialsHolder() { return &m_materialsHolder; }
	TexturesHolder* getTextureHolder() { return &m_texturesHolder; }

	void resetWorkCmdList();
	void executeWorkCmdList();
};