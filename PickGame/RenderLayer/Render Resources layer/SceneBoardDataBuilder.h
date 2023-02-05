#pragma once
#include "..\..\GameBoardLayer\Board.h"
#include "SkinObject.h"

class SceneBoardDataBuilder
{
	SkinDataConnector* m_skinDataConnector;
	SkinObject* m_skinData;
	const GameBoardCurrentStatus* m_boardData;
	bool m_isNewSkinData;
	DirectX::XMFLOAT4X4 m_translationMatrix;

	void loadNewSkin();
	void builScenedBoardData();

	void _BuildTranslationMatrix(const SceneObject& so);
	DirectX::XMFLOAT4X4 _GetDirectionalMatrix(i8 dx, i8 dy, i8 dz);

public:
	SceneBoardDataBuilder();
	~SceneBoardDataBuilder();

	void setConnectorSkinData(SkinDataConnector* connector);
	void setConnectorBoardData(const GameBoardCurrentStatus* boardData);
	bool isNewSkin();
	void resetNewSkinFlag();
	SkinObject* getSkinData();
	void update();
};

