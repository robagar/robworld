#pragma once

#include "TerrainPage.h"


namespace NonUniformTerrain
{
	class Terrain; 

	class ITerrainPageSourceListener
	{
	public:
		virtual void PageDataLoaded(const TerrainPageData* pPageData) PURE;
	};

	class TerrainPageSource
	{
		ITerrainPageSourceListener* m_pListener;

	public:
		TerrainPageSource();
		virtual ~TerrainPageSource();

		void SetListener(ITerrainPageSourceListener* pListener) { m_pListener = pListener; }
		
		virtual void LoadPage(unsigned int id, const TerrainParams& params) PURE;

	protected:
		void OnPageDataLoaded(const TerrainPageData* pPageData);
	};
}
