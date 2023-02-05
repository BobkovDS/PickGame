#pragma once
#include "SkinLoader.h"

class SkinManager
{
	std::vector<SkinObject*> m_skins;
	SkinLoader m_loader;
	SkinDataConnector m_connector;

public:
	SkinManager();
	~SkinManager();
	void setSkin(u8 skin_id);
	void loadSkins(ResourceManager* resourceManager);
	SkinDataConnector* getConnector();
};