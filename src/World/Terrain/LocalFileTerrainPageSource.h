#pragma once

#include "TerrainPageSource.h"

namespace NonUniformTerrain
{
	class LocalFileTerrainPageSource : public TerrainPageSource
	{
		boost::filesystem::path m_pagesDir;

	public:
		LocalFileTerrainPageSource(const Config& config);
		~LocalFileTerrainPageSource(void);

		virtual void LoadPage(unsigned int id, const TerrainParams& params);

	private:
		TerrainPageData* LoadTerrainPageFromFile(unsigned int id);
		std::string GetTerrainPageFileName(unsigned int id);
	};
}
