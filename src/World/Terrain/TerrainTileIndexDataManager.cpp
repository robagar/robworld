#include "../../StdAfx.h"
#include "TerrainTileIndexDataManager.h"
#include "TerrainTileIndexDataManagerListener.h"

using namespace NonUniformTerrain;

TerrainTileIndexDataManager::TerrainTileIndexDataManager(const TerrainParams& terrainParams, TerrainTileIndexDataManagerListener* pListener)
: m_terrainParams(terrainParams),
	m_pListener(pListener),
	m_hardwareBufferManager(Ogre::HardwareBufferManager::getSingleton())
{
}

TerrainTileIndexDataManager::~TerrainTileIndexDataManager()
{
}

size_t TerrainTileIndexDataManager::GetTileIndexBufferSize(short tileLOD)
{
	// n x n quads, no stitching
	int n = 1 << (m_terrainParams.TilePower - tileLOD);
	return n * n * 6;
}

Ogre::IndexData* TerrainTileIndexDataManager::GetIndexData(const TerrainTileParams& params)
{
	Ogre::IndexData* pIndexData;
	bool cacheHit;
	if (contains(m_cache, params))
	{
		pIndexData = m_cache[params]; 
		cacheHit = true;
	}
	else
	{
		pIndexData = ConstructTileIndexData(params);
		m_cache[params] = pIndexData;
		cacheHit = false;
	}
	OnTerrainIndexDataRequested(cacheHit);
	return pIndexData;
}

void TerrainTileIndexDataManager::OnTerrainIndexDataRequested(bool cacheHit)
{
	if (m_pListener != NULL)
		m_pListener->TerrainIndexDataRequested(cacheHit);
}

Ogre::IndexData* TerrainTileIndexDataManager::ConstructTileIndexData(const TerrainTileParams& params)
{
	Ogre::IndexData* pIndexData = new Ogre::IndexData();
	pIndexData->indexBuffer = m_hardwareBufferManager.createIndexBuffer(Ogre::HardwareIndexBuffer::IT_32BIT,
																																			GetTileIndexBufferSize(params.LOD), 
																																			Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

  unsigned int* pIndex = static_cast<unsigned int*>(
		pIndexData->indexBuffer->lock(0, pIndexData->indexBuffer->getSizeInBytes(), Ogre::HardwareBuffer::HBL_DISCARD));

	/*
		Triangle list:

		|    \|    \|    \|
		+-----+-----+-----+--
		|\  1 |\  3 |\  5 |\
		|  \  |  \  |  \  |
		| 0  \| 2  \| 4  \|
		+-----+-----+-----+--
	*/
	short pFirst = m_terrainParams.GetTilePointCoordinate(params.I);  // west
	short pLast = m_terrainParams.GetTilePointCoordinate(params.I+1); // east
	short qFirst = m_terrainParams.GetTilePointCoordinate(params.J);  // south
	short qLast = m_terrainParams.GetTilePointCoordinate(params.J+1); // north

	short step = 1 << params.LOD;

	if (params.MustStitchSouth()) qFirst += step; 
	if (params.MustStitchEast()) pLast -= step; 
	if (params.MustStitchNorth()) qLast -= step; 
	if (params.MustStitchWest()) pFirst += step; 

	int numIndices = 0;
	for (short q=qFirst; q < qLast ; q += step)
	{
		for (short p=pFirst; p < pLast; p += step)
		{
			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, p, q);                numIndices++;
			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, p + step, q);         numIndices++;
			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, p, q + step);         numIndices++;

			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, p, q + step);         numIndices++;
			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, p + step, q);         numIndices++;
			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, p + step, q + step);  numIndices++;
		}
	}

	//debug << "before stitching: " << numIndices << endl;
	if (params.MustStitchSouth()) StitchSouth(params, pIndex, numIndices);
	//debug << "after south: " << numIndices << endl;
	if (params.MustStitchEast()) StitchEast(params, pIndex, numIndices);
	//debug << "after east: " << numIndices << endl;
	if (params.MustStitchNorth()) StitchNorth(params, pIndex, numIndices);
	//debug << "after north: " << numIndices << endl;
	if (params.MustStitchWest()) StitchWest(params, pIndex, numIndices);
	//debug << "after west: " << numIndices << endl;

	int maxIndices = (int)GetTileIndexBufferSize(params.LOD);
	assert(numIndices <= maxIndices && "too many terrain tile indices");

	pIndexData->indexBuffer->unlock();
	pIndexData->indexStart = 0;
	pIndexData->indexCount = numIndices;

	return pIndexData;
}

void TerrainTileIndexDataManager::StitchSouth(const TerrainTileParams& params, unsigned int*& pIndex, int& numIndices)
{
	short loLOD = params.SouthLOD;
	short loStep = 1 << loLOD;
	short loHalfStep = loStep >> 1;
	short hiStep = 1 << params.LOD;
	short pLo = m_terrainParams.GetTilePointCoordinate(params.I);  // west
	short pHi = pLo;
	short pEast = m_terrainParams.GetTilePointCoordinate(params.I+1);  // east
	short qLo = m_terrainParams.GetTilePointCoordinate(params.J);  // south
	short qHi = qLo + hiStep;

	if (!params.MustStitchWest())
	{
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);       numIndices++;
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pLo, qLo);       numIndices++;
		pHi += hiStep;
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);       numIndices++;
	}
	else
		pHi += hiStep;

	for (; pLo < pEast;)
	{
		short pHalfway = pLo + loHalfStep;
		
		// tris to halfway use western lo vertex
		for (; pHi < pHalfway; )
		{
			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);      numIndices++;
			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pLo, qLo);      numIndices++;
			pHi += hiStep;
			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);      numIndices++;
		}

		// middle tri 
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);        numIndices++;
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pLo, qLo);        numIndices++;
		pLo += loStep;
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pLo, qLo);        numIndices++;

		// rest of the tris use eastern lo vertex
		for (; pHi < pLo - hiStep; )
		{
			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);      numIndices++;
			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pLo, qLo);      numIndices++;
			pHi += hiStep;
			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);      numIndices++;
		}
	}

	if (!params.MustStitchEast())
	{
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);        numIndices++;
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pLo, qLo);        numIndices++;
		pHi += hiStep;
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);        numIndices++;
	}
}

void TerrainTileIndexDataManager::StitchEast(const TerrainTileParams& params, unsigned int*& pIndex, int& numIndices)
{
	short loLOD = params.EastLOD;
	short loStep = 1 << loLOD;
	short loHalfStep = loStep >> 1;
	short hiStep = 1 << params.LOD;
	short pLo = m_terrainParams.GetTilePointCoordinate(params.I+1);  // east
	short pHi = pLo - hiStep;
	short qLo = m_terrainParams.GetTilePointCoordinate(params.J);  // north;
	short qHi = qLo;
	short qNorth = m_terrainParams.GetTilePointCoordinate(params.J+1);  // north

	if (!params.MustStitchSouth())
	{
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);       numIndices++;
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pLo, qLo);       numIndices++;
		qHi += hiStep;
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);       numIndices++;
	}
	else
		qHi += hiStep;

	for (; qLo < qNorth;)
	{
		short qHalfway = qLo + loHalfStep;
		
		// tris to halfway use northern lo vertex
		for (; qHi < qHalfway; )
		{
			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);      numIndices++;
			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pLo, qLo);      numIndices++;
			qHi += hiStep;
			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);      numIndices++;
		}

		// middle tri 
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);        numIndices++;
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pLo, qLo);        numIndices++;
		qLo += loStep;
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pLo, qLo);        numIndices++;

		// rest of the tris use southern lo vertex
		for (; qHi < qLo - hiStep; )
		{
			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);      numIndices++;
			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pLo, qLo);      numIndices++;
			qHi += hiStep;
			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);      numIndices++;
		}
	}

	if (!params.MustStitchNorth())
	{
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);        numIndices++;
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pLo, qLo);        numIndices++;
		qHi += hiStep;
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);        numIndices++;
	}
}

void TerrainTileIndexDataManager::StitchNorth(const TerrainTileParams& params, unsigned int*& pIndex, int& numIndices)
{
	short loLOD = params.NorthLOD;
	short loStep = 1 << loLOD;
	short loHalfStep = loStep >> 1;
	short hiStep = 1 << params.LOD;
	short pLo = m_terrainParams.GetTilePointCoordinate(params.I+1);  // east
	short pHi = pLo;
	short qLo = m_terrainParams.GetTilePointCoordinate(params.J+1);  // south
	short qHi = qLo - hiStep;
	short pWest = m_terrainParams.GetTilePointCoordinate(params.I);  // west

	if (!params.MustStitchEast())
	{
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);       numIndices++;
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pLo, qLo);       numIndices++;
		pHi -= hiStep;
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);       numIndices++;
	}
	else
		pHi -= hiStep;

	for (; pLo > pWest;)
	{
		short pHalfway = pLo - loHalfStep;
		
		// tris to halfway use eastern lo vertex
		for (; pHi > pHalfway; )
		{
			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);      numIndices++;
			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pLo, qLo);      numIndices++;
			pHi -= hiStep;
			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);      numIndices++;
		}

		// middle tri 
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);        numIndices++;
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pLo, qLo);        numIndices++;
		pLo -= loStep;
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pLo, qLo);        numIndices++;

		// rest of the tris use western lo vertex
		for (; pHi > pLo + hiStep; )
		{
			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);      numIndices++;
			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pLo, qLo);      numIndices++;
			pHi -= hiStep;
			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);      numIndices++;
		}
	}

	if (!params.MustStitchWest())
	{
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);        numIndices++;
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pLo, qLo);        numIndices++;
		pHi -= hiStep;
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);        numIndices++;
	}
}

void TerrainTileIndexDataManager::StitchWest(const TerrainTileParams& params, unsigned int*& pIndex, int& numIndices)
{
	short loLOD = params.WestLOD;
	short loStep = 1 << loLOD;
	short loHalfStep = loStep >> 1;
	short hiStep = 1 << params.LOD;
	short pLo = m_terrainParams.GetTilePointCoordinate(params.I);  // west
	short pHi = pLo + hiStep;
	short qLo = m_terrainParams.GetTilePointCoordinate(params.J+1);  // south
	short qHi = qLo;
	short qSouth = m_terrainParams.GetTilePointCoordinate(params.J); // south

	if (!params.MustStitchNorth())
	{
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);       numIndices++;
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pLo, qLo);       numIndices++;
		qHi -= hiStep;
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);       numIndices++;
	}
	else
		qHi -= hiStep;

	for (; qLo > qSouth;)
	{
		short qHalfway = qLo - loHalfStep;
		
		// tris to halfway use southern lo vertex
		for (; qHi > qHalfway; )
		{
			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);      numIndices++;
			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pLo, qLo);      numIndices++;
			qHi -= hiStep;
			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);      numIndices++;
		}

		// middle tri 
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);        numIndices++;
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pLo, qLo);        numIndices++;
		qLo -= loStep;
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pLo, qLo);        numIndices++;

		// rest of the tris use northern lo vertex
		for (; qHi > qLo + hiStep; )
		{
			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);      numIndices++;
			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pLo, qLo);      numIndices++;
			qHi -= hiStep;
			*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);      numIndices++;
		}
	}

	if (!params.MustStitchSouth())
	{
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);        numIndices++;
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pLo, qLo);        numIndices++;
		qHi -= hiStep;
		*pIndex++ = m_terrainParams.GetVertexIndex(params.PagePower, pHi, qHi);        numIndices++;
	}
}