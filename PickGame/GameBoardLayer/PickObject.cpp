#include "PickObject.h"


PickObject::PickObject(): m_createdType(0), m_selectedType(0), m_isUsed(false)
{
}

PickObject::~PickObject()
{
}

void PickObject::setType(u8 pickType)
{
	/*	Sets a type for Pick, when game is created. This is a task for a game to guess this value	*/

	m_createdType = pickType;
	m_selectedType = 0;
	m_isUsed = false;
}

void PickObject::tempChoice(u8 typeID)
{
	/* Makes a temporary choice ( a guess) for what is a type for a pick. */
	if (m_isUsed) return;

	m_selectedType = typeID;
}

void PickObject::choose()
{	
	/*	Player has pressed a 'Turn' button, so now we fix a temporary type value for a pick. 
		Pick cannot be selected any more. */

	m_isUsed = true;
}

i8 PickObject::getType()
{
	/* Gets a selected type value for a pick. But we should also show, does it fixed pick or not yet. 
	Render will draw it in different ways. */

	if (m_isUsed)
		return m_selectedType; // Type value for a pick is fixed
	else
		return m_selectedType * (-1); // Type value for a pick is not fixed yet
}

bool PickObject::getChosenStatus()
{
	/* It was right choice for a pick or not? */
	return (m_createdType == m_selectedType);
}