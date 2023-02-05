#include "PSOManager.h"
#include "DXGILayer.h"

PSOManager::PSOManager()
{
	// for default mesh
	{
		auto& layout = m_inputLayouts[PSO_INPUT_LAYOUT_DEFAULT];
		layout.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		layout.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		layout.push_back({ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		layout.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	}

	// For skinned mesh
	{
		auto& layout = m_inputLayouts[PSO_INPUT_LAYOUT_SKINNED];

		layout.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		layout.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		layout.push_back({ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		layout.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		layout.push_back({ "BONEINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 52, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		layout.push_back({ "BONEINDEX", 1, DXGI_FORMAT_R32G32B32A32_UINT, 0, 68, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		layout.push_back({ "BONEINDEX", 2, DXGI_FORMAT_R32G32B32A32_UINT, 0, 84, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		layout.push_back({ "BONEINDEX", 3, DXGI_FORMAT_R32G32B32A32_UINT, 0, 100, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		layout.push_back({ "BONEWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 116, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		layout.push_back({ "BONEWEIGHT", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 132, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		layout.push_back({ "BONEWEIGHT", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 148, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		layout.push_back({ "BONEWEIGHT", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 164, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	}

}

PSOManager::~PSOManager()
{
	auto begin_it = m_psoObjects.begin();
	auto end_it = m_psoObjects.end();

	/*for (auto& pso: m_psoObjects)
		delete pso.second;*/

	m_psoObjects.clear();
}

void PSOManager::buildPSOList(DXGILayerInformation* dxgiInfo)
{
	// Create PSO Object for finalRender
	
	PSOObject& ptrPSOObject = m_psoObjects[PSOOBJECT_NAME_FINAL_RENDER];
	bool compileResult = ptrPSOObject.compileShaderFile(dxgiInfo->pDevice,
		"Shaders/FinalRender.hlsl",
		"vs",
		"VS",
		"vs_5_1",
		NULL);

	compileResult &= ptrPSOObject.compileShaderFile(dxgiInfo->pDevice,
		"Shaders/FinalRender.hlsl",
		"ps",
		"PS",
		"ps_5_1",
		NULL);

	if (!compileResult)
	{
		assert(compileResult && "Error in FinalRender ShaderCompile");
		m_psoObjects.erase(PSOOBJECT_NAME_FINAL_RENDER);
	}
	else
	{
		ptrPSOObject.buildRootSingnature(dxgiInfo->pDevice, "vs");

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = ptrPSOObject.getPSODescriptionDefault();
		psoDesc.VS = ptrPSOObject.getShaderByteCode("vs");
		psoDesc.PS = ptrPSOObject.getShaderByteCode("ps");

		// For Opaque layer
		psoDesc.RTVFormats[0] = dxgiInfo->BackBufferFormat;
		psoDesc.InputLayout = { m_inputLayouts[PSO_INPUT_LAYOUT_DEFAULT].data(), UINT(m_inputLayouts[PSO_INPUT_LAYOUT_DEFAULT].size())};
		ptrPSOObject.createGraphicPSO(dxgiInfo->pDevice, LayerType::LT_opaque, &psoDesc);

		//// For Transparent layer
		//psoDesc.BlendState.RenderTarget[0].BlendEnable = true;
		//psoDesc.BlendState.RenderTarget[0].LogicOpEnable = false;
		//psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		//psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		//psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		//psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		//psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		//psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		//psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		//psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		//ptrPSOObject.createGraphicPSO(dxgiInfo->pDevice, LayerType::LT_transparent, &psoDesc);
	}

	// TO_DO: build error PSOObject
	{

	}
}

PSOObject* PSOManager::getPSOObject(const std::string& psoName)
{
	auto find_it = m_psoObjects.find(psoName);

	if (find_it != m_psoObjects.end())
		return &find_it->second;
	else
		return &m_errorPSOObject;
}