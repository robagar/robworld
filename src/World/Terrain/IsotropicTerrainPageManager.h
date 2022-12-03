#pragma once

#include "TerrainPageManager.h"

namespace NonUniformTerrain
{
	/*
	* Loads all pages within range.
	*/
	class IsotropicTerrainPageManager :	public TerrainPageManager
	{
	public:
		IsotropicTerrainPageManager(Terrain* pTerrain, TerrainPageSource* pSource);
		virtual ~IsotropicTerrainPageManager();

		virtual void BuildRequiredPageList(const TerrainPageManagementParameters& params);

	private:
		void AddPagesWithinSquareToRequiredPageList(float x, float y, float halfWidth);
	};
}
