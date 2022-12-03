#pragma once

namespace NonUniformTerrain
{
	struct TerrainStats
	{
		int Pages;

		int PageLoads;
		int PageUnloads;

		int TileIndexDataRequests;
		int TileIndexDataRequestCacheHits;

		TerrainStats()
		{
			ZeroMemory(this, sizeof(TerrainStats));
		}
	};
}