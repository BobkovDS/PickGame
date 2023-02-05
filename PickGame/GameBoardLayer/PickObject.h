#pragma once
#include "..\Include\datatypedef.h"

class PickObject
{
	u8 m_createdType;
	u8 m_selectedType;
	bool m_isUsed;
public:
	PickObject();
	~PickObject();

	void setType(u8); //set pick type when we build a BoardLine
	void choose(); // fix chosen type value for a pick
	void tempChoice(u8 typeID);	// make a temp choice of pick type
	i8 getType(); // get current type of pick. // TypeValue <0: not fixed choice yet. TypeValue >0: a choice is fixed
	bool getChosenStatus(); //(m_createdType == m_selectedType)
};

