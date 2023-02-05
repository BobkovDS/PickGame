#include "GameBoard.h"
#include "GameRoundTask.h"
#include "..\Others\Logger.h"
#include <string>

using namespace std;

GameBoard::GameBoard()
{
}

GameBoard::~GameBoard()
{
}

void GameBoard::buildBoard(const GameRoundTask& gameRoundTask)
{
	/*
		Given a task for a game (we call it Round), builds a game Board
	*/
	//LogTextBlock block;

	string msg;
	LogTextBlock textBlock;

	msg = "Build a Board(" + std::to_string(gameRoundTask.getLinesCount()) 
		+ "x" + std::to_string(gameRoundTask.getPicksInLineCount()) + ")\n";

	Logger::logln(msg);

	u8 linesCount = gameRoundTask.getLinesCount();
	u8 picksCount = gameRoundTask.getPicksInLineCount();
	const u8* fieldData = gameRoundTask.getFieldData();

	m_board.build(linesCount, picksCount, fieldData);

	updateCurrentGameData();
}

void GameBoard::updateCurrentGameData()
{
	/*Update current Game Data whenever it requires*/

	m_board.getCurrentData(m_gameCurrentState);
}

void GameBoard::makeTempChoiceOfPicks(u8 pickid, u8 pickType)
{
	/* Makes a tamporary choice for selected pick with a selected type*/
	m_board.makeTempChoiceOfPick(pickid, pickType);
	updateCurrentGameData();
}

void GameBoard::makeTurn()
{
	/* A player has pressed the 'Turn' button*/
	m_board.choosePicks();
	updateCurrentGameData();
}

const GameBoardCurrentStatus* GameBoard::getCurrentGameData() const
{	
	/* Provides a game board data for a Render layer*/
	return &m_gameCurrentState;
}