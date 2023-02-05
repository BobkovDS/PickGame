#include <Windows.h>
#include "Others\Logger.h"
#include "Others\GameManager.h"
#include "RenderLayer\Render Resources layer\DirectXObjectsLiveChecker.h"
Logger Logger::m_logger("LogFile");
DirectXObjectsLiveChecker checker;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	GameManager m_gameManager;
	
	m_gameManager.build(hInstance);
}