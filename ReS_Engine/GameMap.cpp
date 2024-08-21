#include "GameMap.h"

void GameMap::SetVertexList()
{
	VertexList.resize(MapWidth * MapHeight);
	int HalfWidth = MapWidth / 2;
	int HalfHeight = MapHeight / 2;

	for (int iRow = 0; iRow < MapHeight; iRow++)
	{
		for (int iCol = 0; iCol < MapWidth; iCol++)
		{
			VertexList[iRow * MapWidth + iCol].Position = { (float)iCol - HalfWidth, 0.0f, (float)HalfHeight - iRow };
			VertexList[iRow * MapWidth + iCol].Color = { 0,0,0,0 };
			VertexList[iRow * MapWidth + iCol].Texture =
			{ ((float)iCol / (float)(MapWidth - 1)) * 10.0f,
			  ((float)iRow / (float)(MapHeight - 1)) * 10.0f };
		}
	}
}

void GameMap::SetIndexList()
{
	IndexList.resize(FaceCount * 3.0f);
	UINT Index = 0;
	for (int Row = 0; Row < MapHeight - 1; Row++)
	{
		for (int Col = 0; Col < MapWidth - 1; Col++)
		{
			UINT NextRow = Row + 1;
			IndexList[Index + 0] = Row * MapWidth + Col;
			IndexList[Index + 1] = IndexList[Index + 0] + 1;
			IndexList[Index + 2] = NextRow * MapWidth + Col;

			IndexList[Index + 3] = IndexList[Index + 2];
			IndexList[Index + 4] = IndexList[Index + 1];
			IndexList[Index + 5] = IndexList[Index + 3] + 1;
			Index += 6;
		}
	}
}

void GameMap::SetMapSize(UINT width, UINT height)
{
	MapWidth = width + 1;
	MapHeight = height + 1;

	FaceCount = width * height * 2.0f;
	CellDistance = 1.0f;
}
