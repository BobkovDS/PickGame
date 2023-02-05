#include "BoardLine.h"

BoardLine::BoardLine(): m_picksCount(0)
{
	m_picks.resize(m_picksCount);
}

BoardLine::~BoardLine()
{
	m_picks.clear();
}


void BoardLine::setLine(u8 picksCount, const u8* picks)
{
	/*
		Build a Board Lines of Picks when a game begin.
	*/

	m_picksCount = picksCount;
	m_picks.resize(m_picksCount);

	for (u8 i = 0; i < m_picksCount; i++)
		m_picks[i].setType(picks[i]);
}

void BoardLine::makeTempChoiceOfPick(u8 pickID, u8 pickType)
{
	/* A player only make a guess about of Pick type*/
	if (pickID >= m_picks.size()) return;

	m_picks[pickID].tempChoice(pickType);
}

void BoardLine::makeFixChoiceOfPick()
{
	/* A player press the 'Turn' button, so we should fix choices for all picks in a line */

	for (u8 i = 0; i < m_picksCount; i++)
		m_picks[i].choose();
}

void BoardLine::getPicksChosenStatuses(std::vector<u8>& statuses)
{
	/*	Provides chosen status for all picks in this line - Were it chosen correct or not.	*/
		
	for (u8 i = 0; i < m_picksCount; i++)
		statuses.push_back( m_picks[i].getChosenStatus());
}

void BoardLine::getPicksSelectedType(std::vector<i8>& pickTypes, u8 lineID)
{
	/* Provides Pick type values for all picks in this line. (temporary and fixed values)	*/
	
	for (u8 i = 0; i < m_picksCount; i++)
		pickTypes[i + m_picksCount * lineID] = m_picks[i].getType();
}