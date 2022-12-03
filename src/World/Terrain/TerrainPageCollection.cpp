#include "../../StdAfx.h"
#include "TerrainPageCollection.h"

using namespace NonUniformTerrain;

TerrainPageCollection::TerrainPageCollection()
{
}

TerrainPageCollection::~TerrainPageCollection()
{
}

void TerrainPageCollection::AddPage(TerrainPage* pPage)
{
	m_pages[pPage->GetID()] = pPage;
}

bool TerrainPageCollection::Contains(int id)
{
	return m_pages.count(id) != 0;
}

bool TerrainPageCollection::Contains(short i, short j)
{
	return Contains(TerrainPage::ID(i, j));
}

TerrainPage* TerrainPageCollection::GetPage(short i, short j)
{
	return m_pages.find(TerrainPage::ID(i, j))->second;
}

