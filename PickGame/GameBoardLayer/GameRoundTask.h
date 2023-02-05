#pragma once
#include "..\Include\datatypedef.h"

class GameRoundTask
{
	u8 m_picksInLineCount;
	u8 m_linesCount;
	u8* m_fieldData;

public:
	GameRoundTask();
	~GameRoundTask();

	void setLineCount(u8 lineCount);
	void setPickInLineCount(u8 pickCount);
	
	u8 getLinesCount() const;
	u8 getPicksInLineCount() const;
	u8* createFieldData();
	const u8* getFieldData() const;
};


inline void GameRoundTask::setLineCount(u8 lineCount)
{
	m_linesCount = lineCount;
}

inline void GameRoundTask::setPickInLineCount(u8 pickCount)
{
	m_picksInLineCount = pickCount;
}

inline u8 GameRoundTask::getLinesCount() const
{
	return m_linesCount;
}

inline u8 GameRoundTask::getPicksInLineCount() const
{
	return m_picksInLineCount;
}

inline const u8* GameRoundTask::getFieldData() const
{
	return m_fieldData;
}
