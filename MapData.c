//------------------------------------------------------------------------------
//
// File Name:	MapData.c
// Author(s):	Gabe Gramblicka (gabriel.gramblicka)
// Course:		GAM150
//
// Copyright � 2023 DigiPen (USA) Corporation.
//
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "MapData.h"
#include "Stream.h"
#include "DGL.h"
#include "Rock.h"
#include "fishengine.h"
#include "EnemySystem.h"
#include "Swarm.h"
#include "Transform.h"
#include "Map.h"
#include <math.h>

#pragma warning( disable: 4244 ) // what is this

//------------------------------------------------------------------------------
// Private Constants:
//------------------------------------------------------------------------------
#define textScaler 23
#define staticWidth 350
#define xMin 200;
//------------------------------------------------------------------------------
// Private Structures:
//------------------------------------------------------------------------------
typedef struct MapData
{
	int numRows;
	int numCols;

}MapData;
//------------------------------------------------------------------------------
// Public Variables:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Private Variables:
//------------------------------------------------------------------------------
static int rows = 0;
static int cols = 0;

//------------------------------------------------------------------------------
// Private Function Declarations:
//------------------------------------------------------------------------------
static Vector2D CalculateWorldPos(int oldRow, int oldCol);
static void MapDataGetSize(Stream stream);
static Vector2D MapDataAddRand();
//------------------------------------------------------------------------------
// Public Functions:
//------------------------------------------------------------------------------

void MapDataRead(Stream stream)
{
	int r = 0;
	while (TRUE)
	{
		const char* token = StreamReadToken(stream);
		for (int c = 0; c < strlen(token); c++)
		{
			if (*(token + c) == '@')
			{
				Vector2D vec = CalculateWorldPos(r, c);
				RockSpawn(vec);
			}
			if (*(token + c) == '%')
			{
				Vector2D vec = CalculateWorldPos(r, c);
				SwarmSpawn(vec);
			}
		}
		r++;
		if (!strcmp(token, ""))
		{
			break;
		}
	}
}

void MapDataBuild(const char* filename)
{
	if (filename)
	{
		Stream stream = StreamOpen(filename);

		if (stream)
		{
			MapDataGetSize(stream);
			StreamClose(&stream);
		}
	}
	if (filename)
	{
		Stream stream = StreamOpen(filename);

		if (stream)
		{
			MapDataRead(stream);
			StreamClose(&stream);
		}
	}
}
//------------------------------------------------------------------------------
// Private Functions:
//------------------------------------------------------------------------------
static Vector2D CalculateWorldPos(int oldRow, int oldCol)
{
	Vector2D newVec;
	newVec.x = (float)(oldCol * textScaler) + 210;
	newVec.y = (float)abs((oldRow * textScaler) - (rows * textScaler));

	Vector2D randVec = MapDataAddRand();
	Vector2DAdd(&newVec, &newVec, &randVec);

	return newVec;
}

static void MapDataGetSize(Stream stream)
{
	// getting the size
	rows = 0;
	cols = 0;
	const char* colCheck = StreamReadToken(stream);
	cols = (int)strlen(colCheck);
	while (TRUE)
	{
		const char* token = StreamReadToken(stream);
		rows++;

		if (!strcmp(token, ""))
		{
			rows++;

			Entity* goal = GoalGetEntity();
			Transform* transform = EntityGetTransform(goal);

			Vector2D goalPos = CalculateWorldPos(0, cols);
			Vector2D endingPos;
			Vector2DSet(&endingPos, 384, goalPos.y - 100);
			TransformSetTranslation(transform, &endingPos);

			break;
		}
	}
}

static Vector2D MapDataAddRand()
{
	float distance = RandomFloat(0.0f, 10.0f);
	float direction = RandomFloat(0.0f, 360.0f);

	Vector2D vec;
	Vector2DSet(&vec, 1, 1);
	Vector2DNormalize(&vec, &vec);
	Vector2DFromAngleDeg(&vec, direction);
	Vector2DScale(&vec, &vec, distance);

	return vec;
}
