#include "SceneBoardDataBuilder.h"
#include "BoundingMath.h"
#include "SceneObject.h"

using namespace std;

SceneBoardDataBuilder::SceneBoardDataBuilder()
	:
	m_skinDataConnector(nullptr),
	m_skinData(nullptr),
	m_boardData(nullptr),
	m_isNewSkinData(false)
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
	}

	// Using GameBoardCurrentStatus data, build new SceneObjects instances
	builScenedBoardData();
}

void SceneBoardDataBuilder::loadNewSkin()
{
	m_skinData = m_skinDataConnector->SkinObjectData;
	assert(m_skinData);

	// For some skin's SceneObjects we need to make a bkup for their Instance data
	vector<SceneObject*> sceneObjects;

	// we need do it for: Skin Digits
	m_skinData->getScore_digits(sceneObjects);
	for (auto& so : sceneObjects)
		so->makeInstancesBkup();
	
	// ... for Skin Left part
	sceneObjects.clear();
	m_skinData->getBoard_leftPart(sceneObjects);
	for (auto& so : sceneObjects)
		so->makeInstancesBkup();

	// ... for Skin Right part
	sceneObjects.clear();
	m_skinData->getBoard_rightPart(sceneObjects);
	for (auto& so : sceneObjects)
		so->makeInstancesBkup();

	// ... for Skin Middle part
	sceneObjects.clear();
	m_skinData->getBoard_middlePart(sceneObjects);
	for (auto& so : sceneObjects)
		so->makeInstancesBkup();

	// ... for Skin Pick
	sceneObjects.clear();
	m_skinData->getBoard_pick(sceneObjects);
	for (auto& so : sceneObjects)
		so->makeInstancesBkup();

	m_isNewSkinData = true;
}

void SceneBoardDataBuilder::builScenedBoardData()
{
	/*
	*	Using Skin and Board data builds fuill Skinned board data every turn
	*/

	// TEMP CREATION
	if (!m_skinData)
		return;

	vector<SceneObject*> sceneObjects;

	m_skinData->getBoard_leftPart(sceneObjects);
	for (auto& so : sceneObjects)
	{
		_BuildTranslationMatrix(*so);

		so->clearInstances();
		auto instancesBkup = so->getInstanceBkup();
		so->addInstance(*instancesBkup);

		for (u8 row =0; row < 2; row++)
			for (u8 col = 0; col < 2; col++)
			{
				InstanceData instancesBkup2 = {};

				instancesBkup2.MaterialId = instancesBkup->MaterialId;
				instancesBkup2.WorldMatrix = _GetDirectionalMatrix( row - 1, 0, col - 1);
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