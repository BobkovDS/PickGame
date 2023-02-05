#pragma once
#include <d3d12.h>
#include "..\..\Include\datatypedef.h"

#include <string>
class CommandFence
{
	ID3D12CommandQueue* m_pCmdQueue;
	ID3D12Fence* m_fence;
	u64 m_fenceValue;
	HANDLE m_fenceEvent;

public:
	CommandFence();
	~CommandFence();
	void create(ID3D12Device*, ID3D12CommandQueue*, u64 initValue, const std::wstring& name);
	void waitForFenceValue(u64 value);
	void flush();
	u64 setFence();
	u64 getCurrentFenceValue();
	
};

inline u64 CommandFence::getCurrentFenceValue()
{
	return m_fenceValue;
}