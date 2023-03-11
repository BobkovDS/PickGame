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

void SkinLoader::loadSkinFiles(std::vector<std::unique_ptr<SkinObject>> &skins, ResourceManager* resourceManager)
{
	/*TODO: Here should be automatic loading existing skin list, so some skin_list_builder
	*/
	if (m_loader)
	{
		resourceManager->resetWorkCmdList();

		std::unique_ptr<SkinObject> newSkin;
		newSkin.reset(new SkinObject());

		m_loader->loadFile("g:/SSDLaba/Programming/DirectX/PickGame/Models/default_skin.pgf");

		if (!m_loader->IsSuccessful())
			return;

		ModelFileConsumer modelFileConsumer(resourceManager, newSkin.get());
		modelFileConsumer.consume(m_loader);
		modelFileConsumer.writeToDevice();

		skins.push_back(std::move(newSkin));

		resourceManager->executeWorkCmdList();
	}
}