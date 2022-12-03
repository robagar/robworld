#include "../../StdAfx.h"
#include "TerrainPageSource.h"

using namespace NonUniformTerrain;


TerrainPageSource::TerrainPageSource()
{
}

TerrainPageSource::~TerrainPageSource()
{
}

void TerrainPageSource::OnPageDataLoaded(const TerrainPageData* pPageData)
{
	m_pListener->PageDataLoaded(pPageData);
}
