#pragma once
#include "PickObject.h"
#include <vector>

class BoardLine
{
	std::vector<PickObject> m_picks;
	u8 m_picksCount;	

public:
	BoardLine();
	~BoardLine();

	void setLine(u8 pickCount, const u8* picks);	
	void makeTempChoiceOfPick(u8 pickID, u8 pickType);
	void makeFixChoiceOfPick();
	void getPicksChosenStatuses(std::vector<u8>& statuses);	
	void getPicksSelectedType(std::vector<i8>& pickTypes, u8 lineID);	
};

