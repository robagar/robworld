#pragma once

namespace NonUniformTerrain
{
  class TerrainPageManager;

  class TerrainPageLoader
  {
    TerrainPageManager* m_pManager;
    int m_pageID;

  public:
    TerrainPageLoader(TerrainPageManager* pManager, int pageID);

    void operator() ();
  };
}
