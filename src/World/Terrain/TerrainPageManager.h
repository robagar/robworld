#pragma once

#include "TerrainParams.h"
#include "TerrainPageSource.h"
#include "TerrainPageManagementParams.h"

namespace NonUniformTerrain
{
	class Terrain;

	class TerrainPageManager : public ITerrainPageSourceListener
	{
		boost::mutex m_mutex;

		Terrain* m_pTerrain;

	protected:
		const TerrainParams& m_terrainParams;
		TerrainPageSource* m_pSource;

		std::set<unsigned int> m_pageIDs;
		std::set<unsigned int> m_requiredPageIDs;
		std::vector<TerrainPage*> m_newPages;
		std::vector<unsigned int> m_toBeRemovedPageIDs;

	protected:
		TerrainPageManager(Terrain* pTerrain, TerrainPageSource* pSource);

	public:
		virtual ~TerrainPageManager();

		void UpdatePages(const TerrainPageManagementParameters& params,
										 std::vector<TerrainPage*>& newPages,
										 std::vector<unsigned int>& toBeRemovedPageIDs);

	protected:
		virtual void BuildRequiredPageList(const TerrainPageManagementParameters& params) PURE;

		virtual void BeginLoadPage(unsigned int id);

	public:
		void LoadPage(unsigned int id);

		// ITerrainPageSourceListener
		void PageDataLoaded(const NonUniformTerrain::TerrainPageData* pPageData);

	private:
		void CreatePage(const TerrainPageData* pPageData);
	};
}
