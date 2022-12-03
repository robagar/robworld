#include "../../StdAfx.h"

#include "TerrainParams.h"
#include "TerrainPage.h"

using namespace NonUniformTerrain;

TerrainParams::TerrainParams(const Config& config)
{
	// zero (ie best) LOD
	ZeroLODPagePower = config.GetSetting<short>("ZeroLODPagePower");
	assert(ZeroLODPagePower != 0);
	ZeroLODPageSize = (1 << ZeroLODPagePower) + 1;
	debug << "  terrain best LOD power: " << ZeroLODPagePower << " (=> page size: " << ZeroLODPageSize << ")" << endl;

	// worst LOD
	WorstLODPagePower = config.GetSetting<short>("WorstLODPagePower");
	assert(WorstLODPagePower != 0);
	WorstLOD = GetLOD(WorstLODPagePower);
	debug << "  terrain worst LOD power: " << WorstLODPagePower 
		<< " (=> worst LOD: " << WorstLOD << ")" << endl;

	// tiling
	PageTilingPower = config.GetSetting<short>("TilingPower");
	assert(PageTilingPower >= 0);
	PageSizeInTiles = 1 << PageTilingPower;
	TilePower = ZeroLODPagePower - PageTilingPower;
	TileSize = (1 << TilePower) + 1; 
	debug << "  terrain tiling power: " << PageTilingPower 
				<< " (=> pages have " << PageSizeInTiles << " per side" 
				<< ", tile size: " << TileSize << ")" << endl;

	// page width
	PageWidth = config.GetSetting<float>("PageWidth");
	debug << "  terrain page width: " << PageWidth << endl;
	
	// vertex lighting
	VertexLightingEnabled = config.GetSetting<bool>("TerrainVertexLightingEnabled");
	LightMapEnabled = config.GetSetting<bool>("TerrainLightMapEnabled");
}

short TerrainParams::GetPageI(float x) const
{
	short iPositive = (short)floor((fabs(x) / PageWidth) + 0.5f);
	return x < 0 ? -iPositive : iPositive;
}

short TerrainParams::GetPageJ(float y) const
{
	short jPositive = (short)floor((fabs(y) / PageWidth) + 0.5f);
	return y < 0 ? -jPositive : jPositive;
}

float TerrainParams::GetPageX(short i) const
{ 
	return i * PageWidth - PageWidth / 2; 
}

float TerrainParams::GetPageY(short j) const
{ 
	return j * PageWidth - PageWidth / 2; 
}

Vector2 TerrainParams::GetPageXY(short i, short j) const
{
  return Vector2(GetPageX(i), GetPageY(j));
}

int TerrainParams::GetPageIDAt(float x, float y) const 
{
	return TerrainPage::ID(GetPageI(x), GetPageJ(y));
}
