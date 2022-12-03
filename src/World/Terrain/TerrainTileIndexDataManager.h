#pragma once

#include "TerrainParams.h"
#include "TerrainTileParams.h"

namespace NonUniformTerrain
{
	class TerrainTileIndexDataManagerListener;

	class TerrainTileIndexDataManager
	{
		const TerrainParams& m_terrainParams;
		Ogre::HardwareBufferManager& m_hardwareBufferManager;
		std::map<TerrainTileParams, Ogre::IndexData*> m_cache;
		TerrainTileIndexDataManagerListener* m_pListener;

	public:
		TerrainTileIndexDataManager(const TerrainParams& terrainParams, TerrainTileIndexDataManagerListener*);
		~TerrainTileIndexDataManager();

		Ogre::IndexData* GetIndexData(const TerrainTileParams& params);

	private:
		Ogre::IndexData* ConstructTileIndexData(const TerrainTileParams& params);

		size_t GetTileIndexBufferSize(short tileLOD);

		void StitchSouth(const TerrainTileParams& params, unsigned int*& pIndex, int& numIndices);
		void StitchEast(const TerrainTileParams& params, unsigned int*& pIndex, int& numIndices);
		void StitchNorth(const TerrainTileParams& params, unsigned int*& pIndex, int& numIndices);
		void StitchWest(const TerrainTileParams& params, unsigned int*& pIndex, int& numIndices);

		void OnTerrainIndexDataRequested(bool cacheHit);
	};
}