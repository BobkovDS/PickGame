#pragma once
#include "RenderBase.h"

class FinalRender : public RenderBase
{
public:
	void build();
	void draw(u32 flags=0);
};