#include "../../StdAfx.h"

#include "Terrain.h"

#include "LocalFileTerrainPageSource.h"
#include "FlatWorldTerrainPageSource.h"
#include "PerlinNoiseTerrainPageSource.h"
#include "IsotropicTerrainPageManager.h"
#include "TerrainTileIndexDataManager.h"

using namespace boost::filesystem;
using namespace NonUniformTerrain;


Terrain::Terrain(TerrainListener* pListener, 
								 Ogre::SceneNode* pSceneNode, 
								 const Config& config)
: m_pListener(pListener),
	m_pSceneNode(pSceneNode),
	m_params(config)
{
	//debug << "Creating terrain..." << endl;

  String terrainPageSourceName = config.GetSetting<String>("TerrainPageSource");
  if (terrainPageSourceName == "FlatWorld")
    m_pPageSource = new FlatWorldTerrainPageSource(config);
  else if (terrainPageSourceName == "PerlinNoise")
    m_pPageSource = new PerlinNoiseTerrainPageSource(config);
  else
	  m_pPageSource = new LocalFileTerrainPageSource(config);
	m_pPageManager = new IsotropicTerrainPageManager(this, m_pPageSource);

	m_pTerrainTileIndexDataManager = new TerrainTileIndexDataManager(m_params, this);

	std::ostringstream o;
	o << "Data/Terrain/Height Colours/" << config.GetSetting<String>("HeightShader") << ".heightcolours";
	m_pHeightShader = new TerrainHeightShader(o.str());
}

Terrain::~Terrain()
{
	delete m_pPageManager;
	delete m_pPageSource;
}

TerrainPage* Terrain::GetPageContaining(float x, float y)
{
	unsigned int pageID = m_params.GetPageIDAt(x,y);
	if (!contains(m_pagesByID, pageID))
		return NULL;
	else
		return m_pagesByID[pageID];
}

float Terrain::GetHeight(float x, float y)
{
	TerrainPage* pPage = GetPageContaining(x,y);
	if (pPage == NULL)
		return 0.0f;
	else
		return pPage->GetHeight(x,y);
}

void Terrain::Update(const Ogre::Vector3& cameraPosition, const Ogre::Vector3& cameraVelocity, float farClipDistance)
{
	// ensure all pages potentially in view are loaded
	TerrainPageManagementParameters params;
	params.CameraPosition = m_pSceneNode->convertWorldToLocalPosition(cameraPosition);
	params.CameraVelocity = m_pSceneNode->convertWorldToLocalPosition(cameraVelocity);
	params.FarClipDistance = farClipDistance;

	std::vector<TerrainPage*> newPages;
	std::vector<unsigned int> unloadedPageIDs;

	m_pagesChanged = false;
	m_pPageManager->UpdatePages(params, newPages, unloadedPageIDs);

	// add new pages
	foreach(TerrainPage* pPage, newPages)
	{
		AddPage(pPage);
	}

	// remove unloaded pages
	foreach(unsigned int& pageID, unloadedPageIDs)
	{
		RemovePage(pageID);
	}

	if (m_pagesChanged)
		UpdateBoundingBox();

	//UpdateStats();
}

void Terrain::AddPage(TerrainPage* pPage)
{
	assert(!contains(m_pagesByID, pPage->GetID()));

	pPage->AddedToTerrain(m_pSceneNode->createChildSceneNode("Terrain Page " + pPage->GetName()), GetPageNeighbours(pPage), m_pTerrainTileIndexDataManager);

	m_pages.push_back(pPage);
	m_pagesByID[pPage->GetID()] = pPage;
	m_pagesChanged = true;

	// update stats
	m_stats.Pages++;
	m_stats.PageLoads++;

  m_pListener->TerrainPageAdded(pPage);
}

void Terrain::RemovePage(unsigned int pageID)
{
	if (!contains(m_pagesByID, pageID))
	{
		// debug << "Warning: attempted to remove terrain page " << pageID << " which does not exist" << endl; 
		return;
	}

	TerrainPage* pPage = GetPage(pageID);
  debug << "Terrain: removing page " << pPage->GetName().c_str() << endl;


	m_pSceneNode->removeAndDestroyChild(pPage->GetSceneNode()->getName());
	m_pages.remove(pPage);
	m_pagesByID.erase(pageID);
	m_pagesChanged = true;

	pPage->RemovedFromTerrain(GetPageNeighbours(pPage));

	// update stats
	m_stats.Pages--;
	m_stats.PageUnloads++;

  m_pListener->TerrainPageRemoved(pPage);

	delete pPage;
}

TerrainPage* Terrain::GetPage(short i, short j)
{
	return GetPage(TerrainPage::ID(i, j));
}

TerrainPage* Terrain::GetPage(unsigned int pageID)
{
	std::map<unsigned int, TerrainPage*>::const_iterator it = m_pagesByID.find(pageID);
	return it == m_pagesByID.end() ? NULL : it->second;
}

std::vector<TerrainPage*> Terrain::GetPageNeighbours(const TerrainPage* pPage)
{
	short i = pPage->GetI();
	short j = pPage->GetJ();

	std::vector<TerrainPage*> neighbours(4);
	neighbours[North] = GetPage(i, j+1);
	neighbours[East] = GetPage(i+1, j);
	neighbours[South] = GetPage(i, j-1);
	neighbours[West] = GetPage(i-1, j);

	return neighbours;
}

void Terrain::UpdateBoundingBox()
{
	if (m_pages.size() == 0)
		m_boundingBox.setExtents(Vector3(0,0,0), Vector3(0,0,0)); 
	else
	{
		Vector3 bbMin(FLOAT_MAX);
		Vector3 bbMax(FLOAT_MIN);

		foreach(TerrainPage* pPage, m_pages)
		{
			bbMin.makeFloor(pPage->GetBoundingBox().getMinimum());
			bbMax.makeCeil(pPage->GetBoundingBox().getMaximum());
		}

		m_boundingBox.setExtents(bbMin, bbMax); 
	}
}

bool Terrain::Intersects(const Ray& ray, Vector3* pIntersectionPoint)
{
	return false;
}

void Terrain::TerrainIndexDataRequested(bool cacheHit)
{
	m_stats.TileIndexDataRequests++;
	if (cacheHit)
		m_stats.TileIndexDataRequestCacheHits++;
}