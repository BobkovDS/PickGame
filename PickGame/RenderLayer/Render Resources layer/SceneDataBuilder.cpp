#include "SceneDataBuilder.h"
#include "ResourceManager.h"
#include "FrameResourceManager.h"

#include "pix3.h"

SceneDataBuilder::SceneDataBuilder():
	m_sceneEnvConnector(nullptr),
	m_resourceManager(nullptr)
{
}

SceneDataBuilder::~SceneDataBuilder()
{
}

void SceneDataBuilder::setConnectorSkinData(SkinDataConnector* connector)
{
	m_skinnedBoardData.setConnectorSkinData(connector);
}

void SceneDataBuilder::setConnectorSceneEnvironment(SceneEnvironmentConnector* connector)
{
	m_sceneEnvConnector = connector;
}

void SceneDataBuilder::setConnectorBoardData(const GameBoardCurrentStatus* boardData)
{
	m_skinnedBoardData.setConnectorBoardData(boardData);
}

void SceneDataBuilder::setResourceManager(ResourceManager* resourceMngr)
{
	assert(resourceMngr);

	m_resourceManager = resourceMngr;
}

void SceneDataBuilder::update()
{
	PIXScopedEvent(PIX_COLOR(155, 100, 50), "SceneDataBuilder_Update");

	assert(m_resourceManager);
	assert(m_sceneEnvConnector);

	m_skinnedBoardData.update();

	if (m_sceneEnvConnector->IsNewData || m_skinnedBoardData.isNewSkin())
	{
		updateSRVData_();

		m_iterator.clear();
		if (m_sceneEnvConnector->SceneEnvData)
			m_iterator.addSceneObjectsHolder(&m_sceneEnvConnector->SceneEnvData->getObjectsHolder());

		if (m_skinnedBoardData.getSkinData())
			m_iterator.addSceneObjectsHolder(&m_skinnedBoardData.getSkinData()->getObjectsHolder());

		m_iterator.build();

		m_sceneEnvConnector->IsNewData = false;
		m_skinnedBoardData.resetNewSkinFlag();
	}

	updateGPUData_();
}

SceneObjectLayerIterator* SceneDataBuilder::getIterator()
{
	return &m_iterator;
}

void SceneDataBuilder::updateGPUData_()
{
	/*
	*	Copy every frame new Instances data to GPU
	*/

	std::vector<InstanceData> instances;

	for (u8 layerID = LayerType::LT_opaque; layerID < LayerType::LT_COUNT; layerID++)
	{
		auto layer = LayerType(layerID);
		u32 instanceCountPerLayer = 0;
		// At first read Environment's Instances (TODO: Make this reading only once)
		{
			if (m_sceneEnvConnector->SceneEnvData)
			{
				auto layerData = m_sceneEnvConnector->SceneEnvData->getObjectsHolder().getLayer(layer);
				if (layerData)
					instanceCountPerLayer += layerData->provideInstances(instances);
			}
		}

		// And then read Board's Instances (TODO: Make this reading only once)
		{
			if (m_skinnedBoardData.getSkinData())
			{
				auto layerData = m_skinnedBoardData.getSkinData()->getObjectsHolder().getLayer(layer);
				if (layerData)
					instanceCountPerLayer += layerData->provideInstances(instances);
			}
		}
	}

	// Now write all Instances to current frame resources on GPU
	auto instancesUploader = m_resourceManager->getFrameResourceManager()->getInstancesCB();

	if (instancesUploader)
		instancesUploader->copyData(0, instances.size(), (BYTE*)instances.data());
}

void SceneDataBuilder::updateSRVData_()
{
	/*
	*	Update Texture SRVs when it is new
	*/

	m_resourceManager->getTextureHolder()->joinSRVs(
		m_sceneEnvConnector->SceneEnvData ? m_sceneEnvConnector->SceneEnvData->getTexturePackId() : 0xFF,
		m_skinnedBoardData.getSkinData() ? m_skinnedBoardData.getSkinData()->getTexturePackId() : 0xFF);
}