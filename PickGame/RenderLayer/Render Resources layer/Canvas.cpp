#include "Canvas.h"
#include <assert.h>
#include <windowsx.h>
#include <string>

#include "pix3.h"

Canvas::Canvas(): m_windowHasBeenCreated(false), m_isFullScreen(false), 
m_canvasIsInitialized(false)
{
	m_width = 1000;
	m_height = 600;
	m_FPS = 0;
	m_averageTime = 0;
	m_averageTimeOut = 0;
}

void Canvas::init(HINSTANCE hInstance)
{
	WNDCLASS wc = {};

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = Canvas::sWindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wc.lpszClassName = L"GPUCanvas";

	ATOM res = RegisterClass(&wc);
	assert(res != 0);

	RECT rect = { 0,0, static_cast<LONG>(m_width), static_cast<LONG>(m_height) };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	m_hwnd = CreateWindow(
		wc.lpszClassName,
		L"Pick Game",
		WS_OVERLAPPEDWINDOW,
		200, 200,
		m_width, m_height,
		NULL, NULL,
		hInstance, this);

	if (m_hwnd)
	{
		ShowWindow(m_hwnd, SW_SHOWNORMAL);
		UpdateWindow(m_hwnd);
		m_canvasIsInitialized = true;
	}
}

void Canvas::run()
{
	assert(m_canvasIsInitialized);

	m_timer.tt_RunStop();

	MSG msg = {};
	PIXBeginEvent(PIX_COLOR(255, 0, 0), "Start frame");

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (m_timer.tick())
			{
				PIXSetMarker(PIX_COLOR(0, 0, 250), "After wait");
				PIXEndEvent();

				PIXBeginEvent(PIX_COLOR(255, 0, 0), "Run frame");
				m_timer.begin();
				work();
				double longTimeInSec = m_timer.end();
				
				PIXSetMarker(PIX_COLOR(0, 0, 180), "Befor wait");
				m_averageTime += longTimeInSec;
				m_FPS++;
				double frameTimeForLimitFPS = 1.0f / 60.0f;// 0.008333;
				if (longTimeInSec < frameTimeForLimitFPS)
				{
					double howLongToWait = (frameTimeForLimitFPS - longTimeInSec);// *1000;
					m_timer.setTickTime(howLongToWait);
				}
			}

			std::string fpsValue("FPS: ");
			if (m_timer.tick1Sec())
			{
				m_FPSOutput = m_FPS;
				m_FPS = 0;

				m_averageTimeOut = m_averageTime / float(m_FPSOutput);
				m_averageTime = 0;
			}

			fpsValue += std::to_string(m_FPSOutput);
			fpsValue += " everageTime: " + std::to_string(m_averageTimeOut);
			SetWindowTextA(m_hwnd, fpsValue.c_str());
		}
	}
}

void Canvas::toggleFullScreen()
{
	m_isFullScreen = !m_isFullScreen;

	if (m_isFullScreen)
	{
		// go to Full Screen 'mode'
		
		// 1) Save current canvas size
		GetWindowRect(m_hwnd, &m_rectBeforeFullScreenToggle);

		// 2) Set new window style
		UINT windowStyle = WS_OVERLAPPEDWINDOW &
			~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
		SetWindowLong(m_hwnd, GWL_STYLE, windowStyle);

		// 3) Get max window size for current monitor resolution
		HMONITOR hMonitor = MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFOEX monitorInfo = {};
		monitorInfo.cbSize = sizeof(MONITORINFOEX);
		GetMonitorInfo(hMonitor, &monitorInfo);

		// 4) Set new maximal size of a window
		SetWindowPos(m_hwnd, HWND_TOP,
			monitorInfo.rcMonitor.left,
			monitorInfo.rcMonitor.top,
			monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
			monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);
		
		// 5) Show a window
		ShowWindow(m_hwnd, SW_MAXIMIZE);
	}
	else
	{
		// come back from Full Screen 'mode'
		// 1) Set new style
		SetWindowLong(m_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

		// 2) Set old size for a window
		SetWindowPos(m_hwnd, HWND_NOTOPMOST,
			m_rectBeforeFullScreenToggle.left,
			m_rectBeforeFullScreenToggle.top,
			m_rectBeforeFullScreenToggle.right - m_rectBeforeFullScreenToggle.left,
			m_rectBeforeFullScreenToggle.bottom - m_rectBeforeFullScreenToggle.top,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);
		
		// 3) Show a window
		ShowWindow(m_hwnd, SW_NORMAL);
	}
}

LRESULT CALLBACK Canvas::sWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Canvas* ptrCanvas = nullptr;

	if (msg == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		ptrCanvas = (Canvas*)pcs->lpCreateParams;
		::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(ptrCanvas));
	}
	else	
		ptrCanvas = reinterpret_cast<Canvas*>(static_cast<LONG_PTR>(::GetWindowLongPtr(hwnd, GWLP_USERDATA)));	

	if (ptrCanvas)
		return ptrCanvas->WindowProc(hwnd, msg, wParam, lParam);

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK Canvas::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
	{
		bool AltPressed = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

		switch (wParam)
		{
		case VK_ESCAPE:
		{
			if (m_isFullScreen)
				toggleFullScreen();

			PostQuitMessage(0);
			return 0;
		}
			
		case VK_RETURN:
			if (AltPressed)
			{
				toggleFullScreen();
				return 0;
			}
		default:
			onKeyDown(wParam);
		}
		break;
	}
	case WM_KEYUP:
	{
		onKeyUp(wParam);
		break;
	}

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		onMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		onMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;

	case WM_MOUSEMOVE:
		onMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	
	case WM_SYSCHAR: // turn off sound for ALT+ENTER
		return 0;
	case WM_SIZE:
		onReSize(LOWORD(lParam), HIWORD(lParam));
		return 0;
	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}