#pragma once
#include "Canvas.h"
#include "DXGILayer.h"
#include "../../Others/Executer.h"

#define FN_VOID_VOID void

class GPUCanvas :
	public Canvas
{
	DXGILayer m_dxgiLayer;
	ExecuterBase* m_ptrWorkFunction;
	ExecuterBase* m_ptrResizeFunction;
	
	bool m_isLeftBtnDown;
	bool m_isRightBtnDown;
	bool m_isWASDPressed;
	POINT m_mouseDownPoint;
	POINT m_dXdY;

	void work() override;
	void onKeyDown(WPARAM btnState) override;
	void onKeyUp(WPARAM btnState) override;
	void onMouseDown(WPARAM btnState, int x, int y) override;
	void onMouseUp(WPARAM btnState, int x, int y) override;
	void onMouseMove(WPARAM btnState, int x, int y) override;
	void onReSize(u16 width, u16 height) override;

	COPY_FORBID(GPUCanvas)

public:
	GPUCanvas();
	~GPUCanvas();

	void init(HINSTANCE hInstance);
	void setResizeFunction(ExecuterBase* f);
	void setWorkFunction(ExecuterBase* f);
	void provideDXGIData(DXGILayerInformation& dxgiData);
	DXGILayer* dxgiLayer();

	bool isLeftBtnMouseDown();
	bool isRightBtnMouseDown();
	bool isWASDPressed();
	POINT getDxDy();
};

inline void GPUCanvas::work()
{
	m_ptrWorkFunction->execute();
}

inline void GPUCanvas::provideDXGIData(DXGILayerInformation& dxgiData)
{
	m_dxgiLayer.provideDXGIInformation(dxgiData);
}

inline DXGILayer* GPUCanvas::dxgiLayer() { return &m_dxgiLayer; };

inline bool GPUCanvas::isLeftBtnMouseDown() { return m_isLeftBtnDown; }
inline bool GPUCanvas::isRightBtnMouseDown() { return m_isRightBtnDown; }
inline bool GPUCanvas::isWASDPressed() { return m_isWASDPressed; }
inline POINT GPUCanvas::getDxDy() 
{ 
	POINT bkup = m_dXdY;
	m_dXdY.x = 0;
	m_dXdY.y = 0;

	return bkup;
}
