#include "GameRoundTaskCreater.h"
#include "..\Others\Logger.h"
#include <ctime>

GameRoundTaskCreater::GameRoundTaskCreater()
{
}

GameRoundTaskCreater::~GameRoundTaskCreater()
{
}

void GameRoundTaskCreater::buildGameRoundTask(const GameRulesPolicy& gameRulesPolicy)
{
	srand(time(0));
	std::string msg;
	
	LogTextBlock textBlock;

	msg = "Build a round task with policy: " + gameRulesPolicy.Name;
	Logger::logln(msg);

	m_gameTask.setLineCount(gameRulesPolicy.LinesCount);
	m_gameTask.setPickInLineCount(gameRulesPolicy.PicksInLineCount);
	u8* fieldData = m_gameTask.createFieldData();
	
	if (fieldData)
	{
		for (u8 line_id = 0; line_id < gameRulesPolicy.LinesCount-1; line_id++)
		{
			fillNewLine(fieldData, gameRulesPolicy);
			fieldData += gameRulesPolicy.PicksInLineCount;
		}
	}	
}

void GameRoundTaskCreater::fillNewLine(u8* fieldData, const GameRulesPolicy& gameRulesPolicy)
{
	int picksCount = gameRulesPolicy.PicksInLineCount; // how many picks in line we have at all
	int picksTypeCount = picksCount - gameRulesPolicy.SameColorCount + 1; // how many unique pick types we have

	u8* samePicks = new u8[picksTypeCount]; // how much picks of same type we have

	for (u8 i = 0; i < picksTypeCount; i++)
		samePicks[i] = 0;

	for (u8 pickId = 0; pickId < picksCount; pickId++)
	{
		int pickTypeId = 0;
		bool goodPickValue = false;
		while (!goodPickValue)
		{
			pickTypeId = rand() % picksTypeCount;
			if (samePicks[pickTypeId] < gameRulesPolicy.SameColorCount)
			{
				samePicks[pickTypeId]++;
				goodPickValue = true;
			}
		}

		fieldData[pickId] = (u8) pickTypeId;
	}

	delete samePicks;
}

const GameRoundTask* GameRoundTaskCreater::getGameRoundTask()
{
	return &m_gameTask;
}