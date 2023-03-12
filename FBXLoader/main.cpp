#include <Windows.h>
#include "Logger.h"
#include "FBXFileLoader.h"
#include "FBXDataConsumer.h"

Logger Logger::m_logger("fbxLoaderLogger.md");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	FBXFileLoader loader;
	loader.loadFile("d:/Laba/Modeling/Programming/Games/PickGame/GamePack/Models/Skins/Default_skin/def_box.fbx");


	if (loader.IsSuccessful())
	{
		FBXDataConsumer consumer;
		consumer.consume(&loader);
		consumer.writeToDevice();
	}
}