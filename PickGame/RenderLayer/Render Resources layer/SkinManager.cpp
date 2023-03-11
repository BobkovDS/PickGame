#include "SkinManager.h"
#include "ResourceManager.h"

SkinManager::SkinManager()
{
}

SkinManager::~SkinManager()
{
	//for (SkinObject* skin : m_skins)
	//	delete skin;
	//
	//m_skins.clear();
}

void SkinManager::loadSkins(ResourceManager* resourceManager)
{
	m_loader.loadSkinFiles(m_skins, resourceManager);
	setSkin(0);
}

void SkinManager::setSkin(u8 skin_id)
{
	if (m_skins.empty() || skin_id >= m_skins.size())
		return;

	m_connector.SkinObjectData = m_skins[skin_id].get();
	m_connector.IsNewSkin = true;
}

SkinDataConnector* SkinManager::getConnector()
{
	return &m_connector;
}