#include "../../StdAfx.h"

#include "TerrainTileParams.h"
#include "TerrainPage.h"

using namespace NonUniformTerrain;

TerrainTileParams::TerrainTileParams(short pagePower, short i, short j)
	: PagePower(pagePower), 
		I(i), J(j), 
		LOD(UNKNOWN_LOD), 
		NorthLOD(UNKNOWN_LOD), EastLOD(UNKNOWN_LOD), SouthLOD(UNKNOWN_LOD), WestLOD(UNKNOWN_LOD)
{
}
