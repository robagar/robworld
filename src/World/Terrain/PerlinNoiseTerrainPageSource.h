#pragma once

#include "TerrainPageSource.h"

namespace NonUniformTerrain
{
	class PerlinNoiseTerrainPageSource : public TerrainPageSource
	{
	public:
		PerlinNoiseTerrainPageSource(const Config& config);
		~PerlinNoiseTerrainPageSource(void);

		virtual void LoadPage(unsigned int id, const TerrainParams& params);

	private:
	};
}
