#pragma once
#include "PSOObject.h"

#define PSOOBJECT_NAME_FINAL_RENDER "final_render_psoobject"

class DXGILayerInformation;

class PSOManager
{
	enum { PSO_INPUT_LAYOUTS_COUNT  = 2 };
	enum { PSO_INPUT_LAYOUT_DEFAULT = 0 };
	enum { PSO_INPUT_LAYOUT_SKINNED = 1 };

	std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayouts[PSO_INPUT_LAYOUTS_COUNT];
	std::map<std::string, PSOObject> m_psoObjects;
	PSOObject m_errorPSOObject; //it is used when PSO with required Name was not found in m_psoObjects

	COPY_FORBID(PSOManager)
public:
	PSOManager();
	~PSOManager();

	void buildPSOList(DXGILayerInformation* dxgiInfo);
	PSOObject* getPSOObject(const std::string& psoName);
};