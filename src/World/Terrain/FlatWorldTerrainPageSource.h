#pragma once

#include "TerrainPageSource.h"

namespace NonUniformTerrain
{
	class FlatWorldTerrainPageSource : public TerrainPageSource
	{
	public:
		FlatWorldTerrainPageSource(const Config& config);
		~FlatWorldTerrainPageSource(void);

		virtual void LoadPage(unsigned int id, const TerrainParams& params);

	private:
	};
}
