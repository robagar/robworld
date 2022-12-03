#include "../../StdAfx.h"
#include "TerrainPageManager.h"
#include "TerrainPageLoader.h"
#include "Terrain.h"

using namespace NonUniformTerrain;

TerrainPageManager::TerrainPageManager(Terrain* pTerrain, TerrainPageSource* pSource)
: m_pTerrain(pTerrain),
	m_terrainParams(pTerrain->Params),
	m_pSource(pSource)
{
	m_pSource->SetListener(this);
}

TerrainPageManager::~TerrainPageManager()
{
}

void TerrainPageManager::UpdatePages(const TerrainPageManagementParameters& params,
																		 std::vector<TerrainPage*>& newPages,
																		 std::vector<unsigned int>& toBeRemovedPageIDs)
{
	// build list of pages required
	m_requiredPageIDs.clear();
	BuildRequiredPageList(params);

	// load pages that are required but not loaded
	//for(TerrainPageIDSetConstIt it = m_requiredPageIDs.begin(); it != m_requiredPageIDs.end(); ++it)
	foreach(const unsigned int& pageID, m_requiredPageIDs)
	{
		if (!contains(m_pageIDs, pageID))
			BeginLoadPage(pageID);
	}

  /*
	// unload pages that are no longer required
	foreach(const unsigned int& pageID, m_pageIDs)
	{
		if (!contains(m_requiredPageIDs, pageID))
			toBeRemovedPageIDs.push_back(pageID);
	}
	foreach(unsigned int& pageID, toBeRemovedPageIDs)
		m_pageIDs.erase(pageID);
  */

	// copy newly created pages collection
	{
		boost::mutex::scoped_lock lock(m_mutex);

		newPages.resize(m_newPages.size());
		copy(m_newPages.begin(), m_newPages.end(), newPages.begin()); 
		m_newPages.clear();
	}
}

void TerrainPageManager::BeginLoadPage(unsigned int id)
{
  String pageName = TerrainPage::GetName(id);
  debug << "TerrainPageManager: begin loading page " << pageName.c_str() << endl;

	// add ID to known pages list to prevent duplicate attempts to load it
	m_pageIDs.insert(id);

	//// load on background thread
	//TerrainPageLoader pageLoader(this, id);
	//boost::thread loaderThread(pageLoader);

	// load on main thread
	LoadPage(id);
}

void TerrainPageManager::LoadPage(unsigned int id)
{
	m_pSource->LoadPage(id, m_pTerrain->Params);
}

void TerrainPageManager::PageDataLoaded(const TerrainPageData* pPageData)
{
	if (pPageData->IsGood())
		CreatePage(pPageData);
}

void TerrainPageManager::CreatePage(const TerrainPageData* pPageData)
{
	String pageName = TerrainPage::GetName(pPageData->I, pPageData->J);
	//debug << "TerrainPageManager: creating page " << pageName.c_str() << endl;

	TerrainPage* pPage = new TerrainPage(m_pTerrain,
																			 *pPageData); 

	{
		boost::mutex::scoped_lock lock(m_mutex);
		m_newPages.push_back(pPage);
	}
}
