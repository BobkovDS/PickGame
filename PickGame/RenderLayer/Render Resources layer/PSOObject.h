#pragma once
#include <winrt/base.h>
#include "../../Include/datatypedef.h"
#include "../../Include/enums.h"
#include "DirectXMath.h"
#include <d3d12.h>
#include <map>


class PSOObject
{
	std::map<std::string, winrt::com_ptr<ID3DBlob>> m_shadersBlob;
	winrt::com_ptr<ID3D12PipelineState> m_pso[LayerType::LT_COUNT];
	winrt::com_ptr<ID3D12PipelineState> m_failedPSO;
	winrt::com_ptr<ID3D12RootSignature> m_rootSignature;
	winrt::com_ptr<ID3D12RootSignature> m_failedRootSignature;

	COPY_FORBID(PSOObject)
public:
	PSOObject();
	~PSOObject();
	
	bool compileShaderFile(ID3D12Device* device,
		const std::string& fileName,
		const std::string& blobName,
		const std::string& entryPointName,
		const std::string& shaderType,
		const D3D_SHADER_MACRO* defines);

	bool buildRootSingnature(ID3D12Device* device, const std::string& shaderBlobName);
	bool createGraphicPSO(ID3D12Device* device, u8 layerID, D3D12_GRAPHICS_PIPELINE_STATE_DESC* psoDesc);
	bool createComputePSO(ID3D12Device* device, u8 layerID, D3D12_COMPUTE_PIPELINE_STATE_DESC* psoDesc);
	D3D12_SHADER_BYTECODE getShaderByteCode(const std::string& shaderBlobName);
	D3D12_GRAPHICS_PIPELINE_STATE_DESC getPSODescriptionDefault();
	ID3D12PipelineState* getPSO(u8 layerID);
	ID3D12RootSignature* getRootSignauture();
};

inline ID3D12RootSignature* PSOObject::getRootSignauture()
{
	return m_rootSignature.get();
}