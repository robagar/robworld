#include "../../StdAfx.h"
#include "TerrainPageLoader.h"
#include "TerrainPageManager.h"

using namespace NonUniformTerrain;

TerrainPageLoader::TerrainPageLoader(TerrainPageManager* pManager, int pageID)
  : m_pManager(pManager), 
    m_pageID(pageID)
{
}

void TerrainPageLoader::operator() ()
{
  m_pManager->LoadPage(m_pageID);
}

