#pragma once

#include "TerrainPageManager.h"
#include "TerrainListener.h"
#include "TerrainStats.h"
#include "TerrainParams.h"
#include "TerrainTileIndexDataManagerListener.h"
#include "TerrainHeightShader.h"

namespace NonUniformTerrain
{
	class TerrainTileIndexDataManager;

	class Terrain : public TerrainTileIndexDataManagerListener
	{
		TerrainListener* m_pListener;
		Ogre::SceneNode* m_pSceneNode;

		TerrainParams m_params;

		std::list<TerrainPage*> m_pages;
		std::map<unsigned int, TerrainPage*> m_pagesByID;
		bool m_pagesChanged;

		Ogre::AxisAlignedBox m_boundingBox;

		TerrainPageSource* m_pPageSource;
		TerrainPageManager* m_pPageManager;

		TerrainTileIndexDataManager* m_pTerrainTileIndexDataManager;

		TerrainStats m_stats;

		TerrainHeightShader* m_pHeightShader;

	public:
		Terrain(TerrainListener* pListener, Ogre::SceneNode* pSceneNode, const Config& config);
		~Terrain();

		const TerrainParams& GetParams() const { return m_params; }
		__declspec(property(get=GetParams)) const TerrainParams& Params;

		float GetHeight(float x, float y);

		void AddPage(TerrainPage* pPage);
		void RemovePage(unsigned int pageID);
		TerrainPage* GetPage(short i, short j);
		TerrainPage* GetPage(unsigned int pageID);
		TerrainPage* GetPageContaining(float x, float y);
		std::vector<TerrainPage*> GetPageNeighbours(const TerrainPage* pPage);

		void Update(const Ogre::Vector3& cameraPosition, const Ogre::Vector3& cameraDirection, float farClipDistance);

		void UpdateBoundingBox();
		const Ogre::AxisAlignedBox& GetBoundingBox() const { return m_boundingBox; }

		bool Intersects(const Ray& ray, Vector3* pIntersectionPoint = NULL);

		TerrainTileIndexDataManager* GetTerrainTileIndexDataManager() const { return m_pTerrainTileIndexDataManager; }

		const TerrainStats& GetStats() const { return m_stats; }

		// TerrainTileIndexDataManagerListener
		virtual void TerrainIndexDataRequested(bool cacheHit);

		const TerrainHeightShader* GetHeightShader() const { return m_pHeightShader; }
		__declspec(property(get=GetHeightShader)) const TerrainHeightShader* HeightShader;

	private:
		//UpdateStats();
	};

}
