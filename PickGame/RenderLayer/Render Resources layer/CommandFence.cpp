#include "CommandFence.h"

CommandFence::CommandFence():
	m_fence(nullptr),
	m_fenceEvent(nullptr),
	m_pCmdQueue(nullptr),
	m_fenceValue(0)
{
}

CommandFence::~CommandFence()
{
	if (m_fence)
		m_fence->Release();

	CloseHandle(m_fenceEvent);
}

void CommandFence::create(ID3D12Device *pDevice, ID3D12CommandQueue *pCmdQueue, u64 initValue, const std::wstring& name)
{
	if (!(pDevice && pCmdQueue)) return;

	m_pCmdQueue = pCmdQueue;
	m_fenceValue = initValue;

	HRESULT res = pDevice->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
	if (SUCCEEDED(res))
	{
		std::wstring fenceName(L"CommandFence");
		fenceName += L" " + name;

		m_fence->SetName(fenceName.c_str());

		m_fenceEvent = CreateEventEx(nullptr, L"CommandFence Event", 0, EVENT_ALL_ACCESS);
	}
}

void CommandFence::waitForFenceValue(u64 value)
{
	if (m_fence->GetCompletedValue() < value)
	{
		m_fence->SetEventOnCompletion(value, m_fenceEvent);
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}
}

u64 CommandFence::setFence()
{	
	/*
		1 - Signal CommandQueue with the current fence value
		2 - Inc current fence value
		3 - return a value with which CommandQueue has been signaled
	*/

	m_pCmdQueue->Signal(m_fence, m_fenceValue);
	m_fenceValue++;
	return m_fenceValue - 1;
}

void CommandFence::flush()
{
	m_pCmdQueue->Signal(m_fence, m_fenceValue);
	waitForFenceValue(m_fenceValue);
	m_fenceValue++;
}