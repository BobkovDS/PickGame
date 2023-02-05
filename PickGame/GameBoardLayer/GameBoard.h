#pragma once
#include "Board.h"
#include "GameRoundTask.h"
class GameBoard
{
	Board m_board;
	GameBoardCurrentStatus m_gameCurrentState;

	void updateCurrentGameData();
public:
	GameBoard();
	~GameBoard();
	
	void buildBoard(const GameRoundTask& gameRoundTask);
	void makeTempChoiceOfPicks(u8 pickId, u8 pickType);
	void makeTurn();
	const GameBoardCurrentStatus* getCurrentGameData() const;
};

