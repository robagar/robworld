#pragma once

namespace NonUniformTerrain
{
	class TerrainTileIndexDataManagerListener
	{
	public:
		virtual void TerrainIndexDataRequested(bool cacheHit) PURE;
	};
}