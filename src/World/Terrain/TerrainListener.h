#pragma once

#include "TerrainPage.h"

class TerrainListener
{
public:
	virtual void TerrainResized(const Ogre::AxisAlignedBox& boundingBox) PURE;

  virtual void TerrainPageAdded(NonUniformTerrain::TerrainPage* pPage) PURE;
  virtual void TerrainPageRemoved(NonUniformTerrain::TerrainPage* pPage) PURE;
};