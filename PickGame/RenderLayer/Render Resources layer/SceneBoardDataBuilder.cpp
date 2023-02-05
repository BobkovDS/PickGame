#include "SceneBoardDataBuilder.h"
#include "BoundingMath.h"
#include "SceneObject.h"

using namespace std;

SceneBoardDataBuilder::SceneBoardDataBuilder()
{
}

SceneBoardDataBuilder::~SceneBoardDataBuilder()
{
}

void SceneBoardDataBuilder::setConnectorSkinData(SkinDataConnector* connector)
{
	m_skinDataConnector = connector;
}

void SceneBoardDataBuilder::setConnectorBoardData(const GameBoardCurrentStatus* boardData)
{
	m_boardData = boardData;
}

bool SceneBoardDataBuilder::isNewSkin()
{
	return m_isNewSkinData;
}

void SceneBoardDataBuilder::resetNewSkinFlag()
{
	m_isNewSkinData = false;
}

SkinObject* SceneBoardDataBuilder::getSkinData()
{
	return m_skinData;
}

void SceneBoardDataBuilder::update()
{
	// set new skin data, if it is required
	if (m_skinDataConnector->IsNewSkin)
	{
		loadNewSkin();
		m_skinDataConnector->IsNewSkin = false;
		m_isNewSkinData = true;
	}

	// Using GameBoardCurrentStatus data, build new SceneObjects instances
	builScenedBoardData();
}

void SceneBoardDataBuilder::loadNewSkin()
{
	m_skinData = m_skinDataConnector->SkinObjectData;
	
	// For some skin's SceneObjects we need to make a bkup for their Instance data
	vector<SceneObject*> lSceneObjects;
	
	// we need do it for: Skin Digits
	m_skinData->getScore_digits(lSceneObjects);
	for (auto& so : lSceneObjects)
		so->makeInstancesBkup();
	
	// ... for Skin Left part
	lSceneObjects.clear();
	m_skinData->getBoard_leftPart(lSceneObjects);
	for (auto& so : lSceneObjects)
		so->makeInstancesBkup();

	// ... for Skin Right part
	lSceneObjects.clear();
	m_skinData->getBoard_rightPart(lSceneObjects);
	for (auto& so : lSceneObjects)
		so->makeInstancesBkup();

	// ... for Skin Middle part
	lSceneObjects.clear();
	m_skinData->getBoard_middlePart(lSceneObjects);
	for (auto& so : lSceneObjects)
		so->makeInstancesBkup();

	// ... for Skin Pick
	lSceneObjects.clear();
	m_skinData->getBoard_pick(lSceneObjects);
	for (auto& so : lSceneObjects)
		so->makeInstancesBkup();
}

void SceneBoardDataBuilder::builScenedBoardData()
{
	// TEMP CREATION

	vector<SceneObject*> lSceneObjects;

	lSceneObjects.clear();
	m_skinData->getBoard_leftPart(lSceneObjects);
	for (auto& so : lSceneObjects)
	{
		_BuildTranslationMatrix(*so);

		so->clearInstances();
		auto instancesBkup = so->getInstanceBkup();
		so->addInstance(*instancesBkup);

		for (u8 row =0; row < 100; row++)
			for (u8 col = 0; col < 100; col++)
			{
				InstanceData instancesBkup2 = {};

				instancesBkup2.MaterialId = instancesBkup->MaterialId;
				instancesBkup2.WorldMatrix = instancesBkup->WorldMatrix;

				instancesBkup2.WorldMatrix = _GetDirectionalMatrix( row - 5, 0, col - 5);
				so->addInstance(instancesBkup2);
			}
	}
}

void SceneBoardDataBuilder::_BuildTranslationMatrix(const SceneObject& so)
{
	DirectX::BoundingBox bb;
	const Mesh* mesh = so.getMesh();
	mesh = mesh ? mesh : so.getMesh_LOD(0);

	assert(mesh);
	DirectX::BoundingBox::CreateFromPoints(
		bb,
		mesh->SubMesh.VertexCount,
		reinterpret_cast<DirectX::XMFLOAT3*>(mesh->VertexCPUBuffer->GetBufferPointer()),
		sizeof(DirectX::XMFLOAT3));

	BoundingMath::BoundingBox myBB(bb);
	DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(myBB.Extents.x *2 , myBB.Extents.y*2, myBB.Extents.z*2);
	DirectX::XMStoreFloat4x4(&m_translationMatrix, translationMatrix);
}

DirectX::XMFLOAT4X4 SceneBoardDataBuilder::_GetDirectionalMatrix(i8 xTimes, i8 yTimes, i8 zTimes)
{
	DirectX::XMFLOAT4X4 resultM = m_translationMatrix;

	resultM._41 *= xTimes;
	resultM._42 *= yTimes;
	resultM._43 *= zTimes;
	
	return resultM;
}