#pragma once
#include "GameRoundTask.h"
#include "GameRulesPolicy.h"

class GameRoundTaskCreater
{
	GameRoundTask m_gameTask;

	void fillNewLine(u8* fieldData, const GameRulesPolicy& gameRulesPolicy);

public:
	GameRoundTaskCreater();
	~GameRoundTaskCreater();

	void buildGameRoundTask(const GameRulesPolicy& gameRulesPolicy);
	const GameRoundTask* getGameRoundTask();
};


