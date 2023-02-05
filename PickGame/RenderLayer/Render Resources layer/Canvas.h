#pragma once

#include <windows.h>
#include "..\..\Include\datatypedef.h"
#include "../../Others/Timer.h"

class Canvas
{
	HWND m_hwnd;
	UINT16 m_width;
	UINT16 m_height;
	RECT m_rectBeforeFullScreenToggle;
	bool m_windowHasBeenCreated;
	bool m_isFullScreen;
	bool m_canvasIsInitialized;
	Timer m_timer;

	int m_FPS;
	int m_FPSOutput;
	float m_averageTime;
	float m_averageTimeOut;
	static LRESULT CALLBACK sWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	COPY_FORBID(Canvas)
protected:
	virtual void work() {};
	virtual void onReSize(u16 width, u16 height);
	virtual void onKeyDown(WPARAM btnState)=0;
	virtual void onKeyUp(WPARAM btnState)=0;
	virtual void onMouseDown(WPARAM btnState, int x, int y) {};
	virtual void onMouseUp(WPARAM btnState, int x, int y) {};
	virtual void onMouseMove(WPARAM btnState, int x, int y) {};

public:
	Canvas();

	void init(HINSTANCE hInstance);
	void run();
	void toggleFullScreen();
	UINT16 getWidth();
	UINT16 getHeight();
	bool isWindowHasBeenCreated();
	HWND getWndHandle();
};

inline UINT16 Canvas::getWidth()
{
	return m_width;
}

inline UINT16 Canvas::getHeight()
{
	return m_height;
}

inline bool Canvas::isWindowHasBeenCreated()
{
	return m_windowHasBeenCreated;
}

inline HWND Canvas::getWndHandle()
{
	return m_hwnd;
}

inline void Canvas::onReSize(u16 width, u16 height)
{
	m_width = width;
	m_height = height;
}