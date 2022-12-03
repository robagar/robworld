#include "../../StdAfx.h"
#include "TerrainPage.h"
#include "TerrainPageData.h"

using namespace NonUniformTerrain;

TerrainPageData::TerrainPageData(unsigned int id)
: ID(id), 
	Status(TerrainPageStatus_NoData), 
	I(TerrainPage::IFromID(id)), 
	J(TerrainPage::JFromID(id)),
	HeightField(NULL)
{
}

TerrainPageData::~TerrainPageData()
{
	SAFE_DELETE_ARRAY(HeightField);
}

void TerrainPageData::Create(short power)
{
	Power = power;
	Size = (1 << power) + 1;
	NumHeightSamples = Size * Size;
	HeightField = new float[NumHeightSamples];
	ZeroMemory(HeightField, NumHeightSamples * sizeof(float));
}