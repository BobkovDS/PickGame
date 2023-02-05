#include "FinalRender.h"
#include "RenderTargetSwapChain.h"
#include "..\Render Resources layer\PSOObject.h"
#include "..\Render Resources layer\ResourceManager.h"
#include "..\Render Resources layer\FrameResourceManager.h"
#include "..\Render Resources layer\TexturesHolder.h"
#include "..\Render Resources layer\MaterialsHolder.h"


void FinalRender::build()
{
}

void FinalRender::draw(u32 flags)
{
	assert(m_renderTargetSwapChain && "RenderTargetSwapChain is Null");

	m_renderTargetSwapChain->setRenderTargetInfo();

	m_cmdList->OMSetRenderTargets(
		1,
		&m_renderTargetSwapChain->getCurrentRTVHeapCPUHandle(),
		true,
		&m_pDSVHeap->GetCPUDescriptorHandleForHeapStart());

	m_cmdList->ClearRenderTargetView(
		m_renderTargetSwapChain->getCurrentRTVHeapCPUHandle(),
		m_renderTargetSwapChain->getClearValue().Color,
		0,
		nullptr);

	m_cmdList->ClearDepthStencilView(m_pDSVHeap->GetCPUDescriptorHandleForHeapStart(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	m_cmdList->OMSetStencilRef(0);

	m_cmdList->SetGraphicsRootSignature(m_PSOObject->getRootSignauture());

	ID3D12DescriptorHeap* descriptorHeaps[] = { m_resourceManager->getTextureHolder()->getSRVHeap() };
	m_cmdList->SetDescriptorHeaps(1, descriptorHeaps);

	u32 techFlags = 0;
	m_cmdList->SetGraphicsRoot32BitConstant(0, techFlags, 1); // Technical Flags
	m_cmdList->SetGraphicsRootShaderResourceView(1,
		m_resourceManager->getFrameResourceManager()->getInstancesCB()->getResource()->GetGPUVirtualAddress());

	// TODO: Add possibility to set empty SRV
	auto boardMaterialPack = m_resourceManager->getMaterialsHolder()->getPack(0);
	if (boardMaterialPack)
		m_cmdList->SetGraphicsRootShaderResourceView(2, boardMaterialPack->getResource()->GetGPUVirtualAddress());

	auto envMaterialPack = m_resourceManager->getMaterialsHolder()->getPack(1);
	if (envMaterialPack)
		m_cmdList->SetGraphicsRootShaderResourceView(3, envMaterialPack->getResource()->GetGPUVirtualAddress());

	m_cmdList->SetGraphicsRootConstantBufferView(4,
		m_resourceManager->getFrameResourceManager()->getMainPassCB()->getResource()->GetGPUVirtualAddress());

	m_cmdList->SetGraphicsRootDescriptorTable(5,
		m_resourceManager->getTextureHolder()->getSRVHeap()->GetGPUDescriptorHandleForHeapStart());

	RenderBase::draw(flags);
}