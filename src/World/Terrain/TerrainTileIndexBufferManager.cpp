#include "../../StdAfx.h"
#include "TerrainTileIndexDataManager.h"

using namespace NonUniformTerrain;

TerrainTileIndexDataManager::TerrainTileIndexDataManager(short tilePower)
: m_hardwareBufferManager(Ogre::HardwareBufferManager::getSingleton()),
	m_tilePower(tilePower)
{
}

TerrainTileIndexDataManager::~TerrainTileIndexDataManager()
{
}

size_t TerrainTileIndexDataManager::GetBufferSize(short tileLOD)
{
	// n x n quads, no stitching
	int n = 1 << (m_tilePower - tileLOD);
	return n * n * 6;
}

Ogre::HardwareIndexBufferSharedPtr& TerrainTileIndexDataManager::GetIndexBuffer(const TerrainPage* pPage, const TerrainTile* pTile)
{
	TerrainTileParams& params = pTile->GetParams();
	if (contains(m_cache, params))
	{
		return m_cache[params]; 
	}
	else
	{
		return ConstructIndexBuffer(params);
	}
}

Ogre::HardwareIndexBufferSharedPtr TerrainTileIndexDataManager::ConstructIndexBuffer(const TerrainTileIndexBufferParams& params)
{
	Ogre::HardwareIndexBufferSharedPtr pIndexBuffer = m_hardwareBufferManager.createIndexBuffer(Ogre::HardwareIndexBuffer::IT_32BIT,
																																															params.GetBufferSize(), 
																																															Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

	return pIndexBuffer;
}

//Ogre::HardwareIndexBufferSharedPtr& TerrainTileIndexDataManager::CacheIndexBuffer(TerrainTileIndexBufferParams& params)
//{
//	int maxIndices = (m_size - 1) * (m_size - 1) * 6; 
//
//	if (m_indexData.indexBuffer.isNull())
//	{
//		m_indexData.indexBuffer = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
//			Ogre::HardwareIndexBuffer::IT_32BIT,
//			maxIndices, 
//			Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
//	}
//
//  unsigned int* pIndex = static_cast<unsigned int*>(
//		m_indexData.indexBuffer->lock(0, m_indexData.indexBuffer->getSizeInBytes(), Ogre::HardwareBuffer::HBL_DISCARD));
//
//	/*
//		Triangle list:
//		+-----+-----+-----+--
//		| 0  /| 2  /| 4  /| 6
//		|  /  |  /  |  /  |  /
//		|/  1 |/  3 |/  5 |/
//		+-----+-----+-----+--
//		|    /|    /|    /|
//	*/
//	short pFirst = m_pWest;
//	short pLast = m_pEast;
//	short qFirst = m_qNorth;
//	short qLast = m_qSouth;
//
//	short step = 1 << m_lod;
//
//	if (MustStitchNorth()) qFirst += step; 
//	if (MustStitchEast()) pLast -= step; 
//	if (MustStitchSouth()) qLast -= step; 
//	if (MustStitchWest()) pFirst += step; 
//
//	int numIndices = 0;
//	for (short q=qFirst; q < qLast ; q += step)
//	{
//		for (short p=pFirst; p < pLast; p += step)
//		{
//			*pIndex++ = m_pPage->GetVertexIndex(p, q);                numIndices++;
//			*pIndex++ = m_pPage->GetVertexIndex(p, q + step);         numIndices++;
//			*pIndex++ = m_pPage->GetVertexIndex(p + step, q);         numIndices++;
//
//			*pIndex++ = m_pPage->GetVertexIndex(p, q + step);         numIndices++;
//			*pIndex++ = m_pPage->GetVertexIndex(p + step, q + step);  numIndices++;
//			*pIndex++ = m_pPage->GetVertexIndex(p + step, q);         numIndices++;
//		}
//	}
//
//	if (MustStitchNorth()) StitchNorth(pIndex, numIndices);
//	if (MustStitchEast()) StitchEast(pIndex, numIndices);
//	if (MustStitchSouth()) StitchSouth(pIndex, numIndices);
//	if (MustStitchWest()) StitchWest(pIndex, numIndices);
//
//	assert(numIndices <= maxIndices && "too many terrain tile indices");
//
//	m_indexData.indexBuffer->unlock();
//	m_indexData.indexStart = 0;
//	m_indexData.indexCount = numIndices;
//}
//
//void TerrainTileIndexDataManager::StitchNorth(unsigned int*& pIndex, int& numIndices)
//{
//	short loLOD = m_neighbourLODs[North];
//	short loStep = 1 << loLOD;
//	short loHalfStep = loStep >> 1;
//	short hiStep = 1 << m_lod;
//	short pLo = m_pWest;
//	short pHi = m_pWest;
//	short qLo = m_qNorth;
//	short qHi = m_qNorth + hiStep;
//
//	if (!MustStitchWest())
//	{
//		*pIndex++ = m_pPage->GetVertexIndex(pLo, qLo);       numIndices++;
//		*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);       numIndices++;
//		pHi += hiStep;
//		*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);       numIndices++;
//	}
//	else
//		pHi += hiStep;
//
//	for (; pLo < m_pEast;)
//	{
//		short pHalfway = pLo + loHalfStep;
//		
//		// tris to halfway use western lo vertex
//		for (; pHi < pHalfway; )
//		{
//			*pIndex++ = m_pPage->GetVertexIndex(pLo, qLo);      numIndices++;
//			*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);      numIndices++;
//			pHi += hiStep;
//			*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);      numIndices++;
//		}
//
//		// middle tri 
//		*pIndex++ = m_pPage->GetVertexIndex(pLo, qLo);        numIndices++;
//		*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);        numIndices++;
//		pLo += loStep;
//		*pIndex++ = m_pPage->GetVertexIndex(pLo, qLo);        numIndices++;
//
//		// rest of the tris use eastern lo vertex
//		for (; pHi < pLo - hiStep; )
//		{
//			*pIndex++ = m_pPage->GetVertexIndex(pLo, qLo);      numIndices++;
//			*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);      numIndices++;
//			pHi += hiStep;
//			*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);      numIndices++;
//		}
//	}
//
//	if (!MustStitchEast())
//	{
//		*pIndex++ = m_pPage->GetVertexIndex(pLo, qLo);        numIndices++;
//		*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);        numIndices++;
//		pHi += hiStep;
//		*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);        numIndices++;
//	}
//}
//
//void TerrainTileIndexDataManager::StitchEast(unsigned int*& pIndex, int& numIndices)
//{
//	short loLOD = m_neighbourLODs[East];
//	short loStep = 1 << loLOD;
//	short loHalfStep = loStep >> 1;
//	short hiStep = 1 << m_lod;
//	short pLo = m_pEast;
//	short pHi = m_pEast - hiStep;
//	short qLo = m_qNorth;
//	short qHi = m_qNorth;
//	
//
//	if (!MustStitchNorth())
//	{
//		*pIndex++ = m_pPage->GetVertexIndex(pLo, qLo);       numIndices++;
//		*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);       numIndices++;
//		qHi += hiStep;
//		*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);       numIndices++;
//	}
//	else
//		qHi += hiStep;
//
//	for (; qLo < m_qSouth;)
//	{
//		short qHalfway = qLo + loHalfStep;
//		
//		// tris to halfway use northern lo vertex
//		for (; qHi < qHalfway; )
//		{
//			*pIndex++ = m_pPage->GetVertexIndex(pLo, qLo);      numIndices++;
//			*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);      numIndices++;
//			qHi += hiStep;
//			*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);      numIndices++;
//		}
//
//		// middle tri 
//		*pIndex++ = m_pPage->GetVertexIndex(pLo, qLo);        numIndices++;
//		*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);        numIndices++;
//		qLo += loStep;
//		*pIndex++ = m_pPage->GetVertexIndex(pLo, qLo);        numIndices++;
//
//		// rest of the tris use southern lo vertex
//		for (; qHi < qLo - hiStep; )
//		{
//			*pIndex++ = m_pPage->GetVertexIndex(pLo, qLo);      numIndices++;
//			*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);      numIndices++;
//			qHi += hiStep;
//			*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);      numIndices++;
//		}
//	}
//
//	if (!MustStitchSouth())
//	{
//		*pIndex++ = m_pPage->GetVertexIndex(pLo, qLo);        numIndices++;
//		*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);        numIndices++;
//		qHi += hiStep;
//		*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);        numIndices++;
//	}
//}
//
//void TerrainTileIndexDataManager::StitchSouth(unsigned int*& pIndex, int& numIndices)
//{
//	short loLOD = m_neighbourLODs[South];
//	short loStep = 1 << loLOD;
//	short loHalfStep = loStep >> 1;
//	short hiStep = 1 << m_lod;
//	short pLo = m_pEast;
//	short pHi = m_pEast;
//	short qLo = m_qSouth;
//	short qHi = m_qSouth - hiStep;
//
//	if (!MustStitchEast())
//	{
//		*pIndex++ = m_pPage->GetVertexIndex(pLo, qLo);       numIndices++;
//		*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);       numIndices++;
//		pHi -= hiStep;
//		*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);       numIndices++;
//	}
//	else
//		pHi -= hiStep;
//
//	for (; pLo > m_pWest;)
//	{
//		short pHalfway = pLo - loHalfStep;
//		
//		// tris to halfway use eastern lo vertex
//		for (; pHi > pHalfway; )
//		{
//			*pIndex++ = m_pPage->GetVertexIndex(pLo, qLo);      numIndices++;
//			*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);      numIndices++;
//			pHi -= hiStep;
//			*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);      numIndices++;
//		}
//
//		// middle tri 
//		*pIndex++ = m_pPage->GetVertexIndex(pLo, qLo);        numIndices++;
//		*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);        numIndices++;
//		pLo -= loStep;
//		*pIndex++ = m_pPage->GetVertexIndex(pLo, qLo);        numIndices++;
//
//		// rest of the tris use western lo vertex
//		for (; pHi > pLo + hiStep; )
//		{
//			*pIndex++ = m_pPage->GetVertexIndex(pLo, qLo);      numIndices++;
//			*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);      numIndices++;
//			pHi -= hiStep;
//			*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);      numIndices++;
//		}
//	}
//
//	if (!MustStitchWest())
//	{
//		*pIndex++ = m_pPage->GetVertexIndex(pLo, qLo);        numIndices++;
//		*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);        numIndices++;
//		pHi -= hiStep;
//		*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);        numIndices++;
//	}
//}
//
//void TerrainTileIndexDataManager::StitchWest(unsigned int*& pIndex, int& numIndices)
//{
//	short loLOD = m_neighbourLODs[West];
//	short loStep = 1 << loLOD;
//	short loHalfStep = loStep >> 1;
//	short hiStep = 1 << m_lod;
//	short pLo = m_pWest;
//	short pHi = m_pWest + hiStep;
//	short qLo = m_qSouth;
//	short qHi = m_qSouth;
//
//	if (!MustStitchSouth())
//	{
//		*pIndex++ = m_pPage->GetVertexIndex(pLo, qLo);       numIndices++;
//		*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);       numIndices++;
//		qHi -= hiStep;
//		*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);       numIndices++;
//	}
//	else
//		qHi -= hiStep;
//
//	for (; qLo > m_qNorth;)
//	{
//		short qHalfway = qLo - loHalfStep;
//		
//		// tris to halfway use southern lo vertex
//		for (; qHi > qHalfway; )
//		{
//			*pIndex++ = m_pPage->GetVertexIndex(pLo, qLo);      numIndices++;
//			*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);      numIndices++;
//			qHi -= hiStep;
//			*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);      numIndices++;
//		}
//
//		// middle tri 
//		*pIndex++ = m_pPage->GetVertexIndex(pLo, qLo);        numIndices++;
//		*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);        numIndices++;
//		qLo -= loStep;
//		*pIndex++ = m_pPage->GetVertexIndex(pLo, qLo);        numIndices++;
//
//		// rest of the tris use northern lo vertex
//		for (; qHi > qLo + hiStep; )
//		{
//			*pIndex++ = m_pPage->GetVertexIndex(pLo, qLo);      numIndices++;
//			*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);      numIndices++;
//			qHi -= hiStep;
//			*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);      numIndices++;
//		}
//	}
//
//	if (!MustStitchNorth())
//	{
//		*pIndex++ = m_pPage->GetVertexIndex(pLo, qLo);        numIndices++;
//		*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);        numIndices++;
//		qHi -= hiStep;
//		*pIndex++ = m_pPage->GetVertexIndex(pHi, qHi);        numIndices++;
//	}
//}