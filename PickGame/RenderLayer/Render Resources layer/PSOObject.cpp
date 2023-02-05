#include "PSOObject.h"
#include "Utilit3D.h"
#include "../d3dx12.h"

using namespace std;

PSOObject::PSOObject()
{
}

PSOObject::~PSOObject()
{
}

bool PSOObject::compileShaderFile(ID3D12Device* device,
	const std::string& fileName,
	const std::string& blobName,
	const std::string& entryPointName,
	const std::string& shaderType,
	const D3D_SHADER_MACRO* defines)
{
	/*
	*	True - OK
	*	False - Error
	*/

	return Utilit3D::compileShader(fileName, defines, entryPointName, shaderType, m_shadersBlob[blobName].put());
}

D3D12_SHADER_BYTECODE PSOObject::getShaderByteCode(const string& shaderBlobName)
{
	D3D12_SHADER_BYTECODE result = {nullptr, 0};

	auto find_it = m_shadersBlob.find(shaderBlobName);

	if (find_it != m_shadersBlob.end())
	{
		result.pShaderBytecode = reinterpret_cast<BYTE*> (find_it->second.get()->GetBufferPointer());
		result.BytecodeLength = find_it->second.get()->GetBufferSize();
	}

	return result;
}

bool PSOObject::buildRootSingnature(ID3D12Device* device, const string& shaderBlobName)
{
	/*
		return:
		True - RootSignature has been created
		False - RootSingnature has been NOT created
	*/

	if (getRootSignauture() == nullptr) // if we did not create a RootSingnature yet
	{
		auto find_it = m_shadersBlob.find(shaderBlobName);
		if (find_it != m_shadersBlob.end())
		{
			m_rootSignature.put();

			winrt::check_hresult(
				device->CreateRootSignature(0,
					find_it->second.get()->GetBufferPointer(),
					find_it->second.get()->GetBufferSize(),
					IID_PPV_ARGS(m_rootSignature.put()))
			);

			return true;
		}
	}
	
	return false;
}

D3D12_GRAPHICS_PIPELINE_STATE_DESC PSOObject::getPSODescriptionDefault()
{
	/*
	Return default PSO description:
		- Stencil test is turn off
		- Blending is turn off
		- FillMode is D3D12_FILL_MODE_SOLID
		- FrontCounterClockwise is FALSE
	*/

	assert(m_rootSignature.get() != nullptr); // buildRootSingnature() should be call before this

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDescription = {};

	psoDescription.pRootSignature = m_rootSignature.get();

	psoDescription.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDescription.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	psoDescription.RasterizerState.FrontCounterClockwise = false;
	psoDescription.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	psoDescription.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDescription.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDescription.SampleMask = UINT_MAX;
	psoDescription.SampleDesc.Count = 4;
	psoDescription.SampleDesc.Quality= 0;

	psoDescription.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDescription.NumRenderTargets = 1;
	psoDescription.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	return psoDescription;
}

bool PSOObject::createGraphicPSO(ID3D12Device* device, u8 layerId, D3D12_GRAPHICS_PIPELINE_STATE_DESC* psoDesc)
{
	if (layerId <= LayerType::LT_COUNT)
	{
		//m_pso[layerId] = nullptr; // If it is already set
		// TODO: Is old PSO will be deleted?
		winrt::check_hresult(
			device->CreateGraphicsPipelineState(psoDesc, IID_PPV_ARGS(m_pso[layerId].put()))
		);

		return true;
	}

	return false;
}

bool PSOObject::createComputePSO(ID3D12Device* device, u8 layerId, D3D12_COMPUTE_PIPELINE_STATE_DESC* psoDesc)
{
	bool result = false;

	if (layerId <= LayerType::LT_COUNT)
	{
		//m_pso[layerId] = nullptr; // If it is already set // TO_DO: Is old PSO will be deleted?
		winrt::check_hresult(
			device->CreateComputePipelineState(psoDesc, IID_PPV_ARGS(m_pso[layerId].put()))
		);

		return true;
	}

	return result;
}

ID3D12PipelineState* PSOObject::getPSO(u8 layerID)
{
	if (layerID <= LayerType::LT_COUNT)
		return m_pso[layerID].get();
	else
		return m_failedPSO.get();
}