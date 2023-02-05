#pragma once
#include "FrameResource.h"
#include "CommandFence.h"

const u8 g_FrameResourceCount = 3;

class FrameResourceManager
{
	FrameResource m_frameResources[g_FrameResourceCount];
	u8 m_currentFRId;
	FrameResource* m_currentFR;
	CommandFence* m_pCmdFence;
	bool m_initialized;
	bool m_newFrame;

	COPY_FORBID(FrameResourceManager)
public:
	FrameResourceManager();
	~FrameResourceManager();

	void initialize(ID3D12Device* pDevice, CommandFence* pCmdFence);
	void getFreeFR();
	void frameDone();
	void changeCmdAllocator(ID3D12GraphicsCommandList* cmdList, ID3D12PipelineState* pso);
	UploadBufferResource* getUploadBuffer(const std::string& bufferName);
	UploadBufferResource* getMainPassCB(); // specific version for getUploadBuffer()
	UploadBufferResource* getInstancesCB(); // specific version for getUploadBuffer()
	void addUploadBuffer(const std::string& bufferName, bool isConstant = false, u32 elementCount = 1);
	void resizeUploadBuffer(const std::string& name, u32 elementCount);
	
};