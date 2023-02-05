#include "SkinLoader.h"
#include "PGFFileLoader.h"
#include "ModelFileConsumer.h"
#include "ResourceManager.h"

using namespace std;

SkinLoader::SkinLoader():
	m_loader(nullptr)
{
	m_loader = new PGFFileLoader();
}

SkinLoader::~SkinLoader()
{
	if (m_loader)
		delete m_loader;
}

void SkinLoader::loadSkinFiles(vector<SkinObject*>& skins, ResourceManager* resourceManager)
{
	if (m_loader)
	{
		resourceManager->resetWorkCmdList();

		SkinObject* newSkin = new SkinObject(); // The skin will be deleted by SkinManager

		m_loader->loadFile("g:/SSDLaba/Programming/DirectX/PickGame/Models/default_skin.pgf");

		ModelFileConsumer modelFileConsumer(resourceManager, newSkin);
		modelFileConsumer.consume(m_loader);
		modelFileConsumer.writeToDevice();

		skins.push_back(newSkin);

		resourceManager->executeWorkCmdList();
	}
}