#pragma once

class GPUCanvas;
class Camera;

class IOManager
{
	GPUCanvas* m_canvas;
	Camera* m_camera;

public:
	void initialize(GPUCanvas* canvas);
	void setCamera(Camera* camera);
	void update();
};