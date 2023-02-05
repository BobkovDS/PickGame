#include "GameRoundTask.h"


GameRoundTask::GameRoundTask():
	m_picksInLineCount(0),
	m_linesCount(0),
	m_fieldData(nullptr)

{
}

GameRoundTask::~GameRoundTask()
{
	if (m_fieldData)
		delete[] m_fieldData;

	m_fieldData = 0;
}

u8* GameRoundTask::createFieldData()
{
	// delete old field, if we have it
	if (m_fieldData)
		delete[] m_fieldData;

	// field cannot be 0 size
	if (m_linesCount <= 0 | m_picksInLineCount <= 0)
		return nullptr;

	// create new field
	m_fieldData = new u8[m_linesCount*m_picksInLineCount];
	
	return m_fieldData;
}

