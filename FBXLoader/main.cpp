#include <Windows.h>
#include "Logger.h"
#include "FBXFileLoader.h"
#include "FBXDataConsumer.h"

//Logger Logger::m_logger("LogFile");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	FBXFileLoader loader;
	loader.loadFile("g:/SSDLaba/Programming/DirectX/PickGame/Models/default_skin.fbx");

	if (loader.isGood())
	{
		FBXDataConsumer consumer;
		consumer.consume(&loader);
		consumer.writeToDevice();
	}
}