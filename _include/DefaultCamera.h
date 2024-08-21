#pragma once
#include "MathLib.h"
class DefaultCamera
{
public:
	virtual bool Init();
	virtual bool Frame();
	virtual bool Render();
	virtual bool Release();
};

