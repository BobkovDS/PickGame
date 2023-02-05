#pragma once
#include <winrt\base.h>
#include <dxgidebug.h>
#include <dxgi1_4.h>

struct DirectXObjectsLiveChecker
{
	~DirectXObjectsLiveChecker()
	{
#if defined(_DEBUG)
		{
			winrt::com_ptr<IDXGIDebug1> debugController;
			if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(debugController.put()))))
				debugController->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		}
#endif
	}
};