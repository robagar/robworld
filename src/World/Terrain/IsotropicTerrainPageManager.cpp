#include "../../StdAfx.h"
#include "Terrain.h"
#include "IsotropicTerrainPageManager.h"

using namespace NonUniformTerrain;

IsotropicTerrainPageManager::IsotropicTerrainPageManager(Terrain* pTerrain, TerrainPageSource* pSource)
: TerrainPageManager(pTerrain, pSource)
{
}

IsotropicTerrainPageManager::~IsotropicTerrainPageManager()
{
}

void IsotropicTerrainPageManager::BuildRequiredPageList(const TerrainPageManagementParameters& params)
{
	AddPagesWithinSquareToRequiredPageList(params.CameraPosition.x, params.CameraPosition.y, params.FarClipDistance);
}

void IsotropicTerrainPageManager::AddPagesWithinSquareToRequiredPageList(float x, float y, float halfWidth)
{
	short minI = m_terrainParams.GetPageI(x - halfWidth); 
	short maxI = m_terrainParams.GetPageI(x + halfWidth); 
	short minJ = m_terrainParams.GetPageJ(y - halfWidth); 
	short maxJ = m_terrainParams.GetPageJ(y + halfWidth);

	for (short j=minJ; j<=maxJ; ++j)
	{
		for (short i=minI; i<=maxI; ++i)
		{
			unsigned int id = TerrainPage::ID(i,j);
			assert(m_requiredPageIDs.find(id) == m_requiredPageIDs.end());
			m_requiredPageIDs.insert(id);
		}
	}
}

