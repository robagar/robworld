#include "../../StdAfx.h"
#include "../../Misc/Config.h"
#include "FlatWorldTerrainPageSource.h"


using namespace NonUniformTerrain;

FlatWorldTerrainPageSource::FlatWorldTerrainPageSource(const Config& config)
: TerrainPageSource()
{
}

FlatWorldTerrainPageSource::~FlatWorldTerrainPageSource()
{
}

void FlatWorldTerrainPageSource::LoadPage(unsigned int id, const TerrainParams& params)
{
	TerrainPageData* pPageData = new TerrainPageData(id);
  pPageData->Create(4);
  pPageData->Status = TerrainPageStatus_OK;
	OnPageDataLoaded(pPageData);
}


