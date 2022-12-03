#include "../../StdAfx.h"

#include "NonUniformTerrain.h"

#include "LocalFileTerrainPageSource.h"
#include "IsotropicTerrainPageManager.h"

using namespace boost::filesystem;
using namespace NonUniformTerrain;

Terrain::Terrain(const Config& config)
{
	m_pageWidth = config.GetSetting<float>("PageWidth");
	m_pPageSource = new LocalFileTerrainPageSource(config);
	m_pPageManager = new IsotropicTerrainPageManager(this, m_pPageSource);
}

Terrain::~Terrain()
{
	delete m_pPageManager;
	delete m_pPageSource;
}

short Terrain::GetPageI(float x)
{
	short iPositive = (short)floor((fabs(x) / m_pageWidth) + 0.5f);
	return x < 0 ? -iPositive : iPositive;
}

short Terrain::GetPageJ(float z)
{
	// since pages are square can use the horizontal calculation
	return GetPageI(z);
}

int Terrain::GetPageIDAt(float x, float z)
{
	return TerrainPage::ID(GetPageI(x), GetPageJ(z));
}

void Terrain::Update(const Ogre::Vector3& cameraPosition, const Ogre::Vector3& cameraVelocity, float farClipDistance)
{
	// ensure all pages potentially in view are loaded
	TerrainPageManagementParameters params;
	params.CameraPosition = cameraPosition;
	params.CameraVelocity = cameraVelocity;
	params.FarClipDistance = farClipDistance;

	vector<TerrainPageData*> newPageData;
	vector<int> unloadedPageIDs;

	m_pPageManager->UpdatePages(params, newPageData, unloadedPageIDs);

	// add new pages
	for (vector<TerrainPageData*>::const_iterator it = newPageData.begin(); it != newPageData.end(); ++it)
	{
		TerrainPageData* pPageData = *it;
		debug << "NonUniformTerrain: adding page " << pPageData->I << ", " << pPageData->J << endl;
		m_pages.AddPage(new TerrainPage(*pPageData));
		delete pPageData;
	}

	// remove unloaded pages
	// TODO
}


