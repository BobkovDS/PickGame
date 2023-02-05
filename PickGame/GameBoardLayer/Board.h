#pragma once
#include "BoardLine.h"
#include <vector>

struct GameBoardCurrentStatus
{
	u8 LinesCount;
	u8 PicksInLineCount;
	u8 CurrentActiveLineID;
	bool GameIsOver;
	bool NeedToUpdate;
	const i8* PickTypes;
	const u8* PickChosenStatus;
};

class Board
{
	std::vector<BoardLine> m_lines;
	std::vector<i8> m_pickTypes;
	std::vector<u8> m_pickChosenStatuses;

	u8 m_linesCount;
	u8 m_picksInLineCount;
	u8 m_currentActiveLineID;

public:
	Board();
	~Board();
	
	void build(u8 linesCount, u8 picksCount, const u8* roundTask);
	void makeTempChoiceOfPick(u8 pickId, u8 pickType);
	void getCurrentData(GameBoardCurrentStatus& currentData);
	bool choosePicks();
};

