#include "GPUCanvas.h"
#include <dxgidebug.h>
GPUCanvas::GPUCanvas(): 
	m_ptrWorkFunction(nullptr),
	m_ptrResizeFunction(nullptr),
	m_isLeftBtnDown(false),
	m_isRightBtnDown(false),
	m_isWASDPressed(false),
	m_mouseDownPoint(POINT()),
	m_dXdY(POINT())
{
}

GPUCanvas::~GPUCanvas()
{
	if (m_ptrWorkFunction)
		delete m_ptrWorkFunction;

	if (m_ptrResizeFunction)
		delete m_ptrResizeFunction;
}

void GPUCanvas::init(HINSTANCE hInstance)
{
	Canvas::init(hInstance);
	m_dxgiLayer.init(getWndHandle(), getWidth(), getHeight());
}

void GPUCanvas::setResizeFunction(ExecuterBase* f)
{
	m_ptrResizeFunction = f;
}

void GPUCanvas::setWorkFunction(ExecuterBase* f)
{
	m_ptrWorkFunction = f;
}

void GPUCanvas::onKeyDown(WPARAM btnStatus)
{

	switch (btnStatus)
	{
	case VK_SPACE:
		break;
	case 'W':
	case 'A':
	case 'S':
	case 'D':
		m_isWASDPressed = true;
		break;

	default:
		break;
	}
}

void GPUCanvas::onKeyUp(WPARAM btnStatus)
{

	switch (btnStatus)
	{
	case 'W':
	case 'A':
	case 'S':
	case 'D':
		m_isWASDPressed = false;
		break;

	default:
		break;
	}
}
void GPUCanvas::onMouseDown(WPARAM btnState, int x, int y)
{
	m_isLeftBtnDown = false;
	m_isRightBtnDown = false;
	

	if (btnState == MK_LBUTTON)
	{
		m_isLeftBtnDown = true;
		m_mouseDownPoint.x = x;
		m_mouseDownPoint.y = y;
	}

	if (btnState == MK_RBUTTON)
		m_isRightBtnDown = true;
}


void GPUCanvas::onMouseUp(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) == 0 )
		m_isLeftBtnDown = false;

	if ((btnState & MK_RBUTTON) ==0 )
		m_isRightBtnDown = false;
}

void GPUCanvas::onMouseMove(WPARAM btnState, int x, int y)
{
	if (m_isLeftBtnDown)
	{
		m_dXdY.x = x - m_mouseDownPoint.x;
		m_dXdY.y = y - m_mouseDownPoint.y;

		m_mouseDownPoint.x = x;
		m_mouseDownPoint.y = y;
	}
}


void GPUCanvas::onReSize(u16 width, u16 height)
{
	if (getWidth() == width && getHeight() == height)
		return;

	Canvas::onReSize(width, height);
	m_dxgiLayer.resize(width, height);
	
	if (m_ptrResizeFunction)
		m_ptrResizeFunction->execute((int) width, (int)height);
}
