#pragma once
#include "BaseObject.h"
class GameMap : public BaseObject
{
protected:
    UINT MapWidth;
    UINT MapHeight;
public:
    int FaceCount;
    int CellCount;
    float CellDistance;

public:
    virtual void SetVertexList();
    void SetIndexList();
    void SetMapSize(UINT width, UINT height);
};

