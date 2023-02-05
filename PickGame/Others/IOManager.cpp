#include "IOManager.h"
#include "../RenderLayer/Render Resources layer/GPUCanvas.h"
#include "../RenderLayer/Render Resources layer/Camera.h"

void IOManager::initialize(GPUCanvas* canvas)
{
	assert(canvas);
	m_canvas = canvas;
}

void IOManager::setCamera(Camera* camera)
{
	assert(camera);
	m_camera = camera;
}

void IOManager::update()
{
	bool needUpdate = false;

	if (m_canvas->isWASDPressed())
	{
		float dt = 0.05f;
		float da = DirectX::XMConvertToRadians(0.5f);

		if (GetAsyncKeyState('W') & 0x8000)
			m_camera->walk(dt);

		if (GetAsyncKeyState('S') & 0x8000)
			m_camera->walk(-dt);

		if (GetAsyncKeyState('A') & 0x8000)
			m_camera->strafe(-dt);

		if (GetAsyncKeyState('D') & 0x8000)
			m_camera->strafe(dt);

		needUpdate = true;
	}

	if (m_canvas->isLeftBtnMouseDown())
	{
		POINT dxdy = m_canvas->getDxDy();
		float dx = DirectX::XMConvertToRadians(0.25f * float(dxdy.x));
		float dy = DirectX::XMConvertToRadians(0.25f * float(dxdy.y));

		m_camera->pitch(dy);
		m_camera->rotateY(dx);

		needUpdate = true;
	}

	if (needUpdate)
		m_camera->updateViewMatrix();
}