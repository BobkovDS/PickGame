#pragma once
#include "..\Include\datatypedef.h"
#include <string>
#include <iostream>

struct GameRulesPolicy
{
	u8 PicksInLineCount;
	u8 LinesCount;
	u8 SameColorCount;
	u8 DifficultyLevel;
	std::string Name;
};