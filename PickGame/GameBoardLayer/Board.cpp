#include "Board.h"

Board::Board(): m_currentActiveLineID(0), m_linesCount(0), m_picksInLineCount(0)
{
	m_lines.resize(m_linesCount);
}

Board::~Board()
{
	m_lines.clear();
}


void Board::build(u8 linesCount, u8 picksInLineCount, const u8* roundTask)
{
	/*
		Creates a game field with <linesCount> rows, with <picksInLineCount> picks in each row. 
		The type of a pick is defined in <roundTask>
	*/	

	m_lines.resize(linesCount);
	const u8* beginPtr = roundTask;

	for (u8 l = 0; l < linesCount; l++)
	{
		beginPtr += l * picksInLineCount;
		m_lines[l].setLine(picksInLineCount, beginPtr);
	}

	m_linesCount = linesCount;
	m_picksInLineCount = picksInLineCount;
	m_currentActiveLineID = 0;

	m_pickTypes.resize(m_picksInLineCount);
}

void Board::makeTempChoiceOfPick(u8 pickId, u8 pickType)
{	
	/*Makes a temporary choice of selected pick in current active line*/

	m_lines[m_currentActiveLineID].makeTempChoiceOfPick(pickId, pickType);
	m_lines[m_currentActiveLineID].getPicksSelectedType(m_pickTypes, m_currentActiveLineID);
}

bool Board::choosePicks()
{
	/* 
		Make fix choice for a current line. 
		Add in vector of already selected Pick's Types types values for picks of this current line
		Add in vector of already chosen Pick's chosen statuses statuses for picks of this current line
		Go to the next line. If lines are over, gives know about that (true - game is done)		
	*/
	if (m_currentActiveLineID == m_lines.size()) return true;

	m_lines[m_currentActiveLineID].makeFixChoiceOfPick();

	m_lines[m_currentActiveLineID].getPicksSelectedType(m_pickTypes, m_currentActiveLineID);
	m_lines[m_currentActiveLineID].getPicksChosenStatuses(m_pickChosenStatuses);

	m_currentActiveLineID++;
	bool isGameDone = (m_currentActiveLineID == m_lines.size() ? true : false);

	if (!isGameDone)
	{
		m_pickTypes.resize(m_pickTypes.size() + m_picksInLineCount);
		m_lines[m_currentActiveLineID].getPicksSelectedType(m_pickTypes, m_currentActiveLineID);
	}
	
	return isGameDone;
}

void Board::getCurrentData(GameBoardCurrentStatus& currentData)
{
	currentData.LinesCount = m_linesCount;
	currentData.PicksInLineCount = m_picksInLineCount;
	currentData.CurrentActiveLineID = m_currentActiveLineID;
	currentData.GameIsOver = (m_currentActiveLineID == m_lines.size() ? true : false);
	currentData.PickTypes = m_pickTypes.data();
	currentData.PickChosenStatus = m_pickChosenStatuses.data();
}