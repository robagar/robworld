#include "../../StdAfx.h"
#include "../../Misc/Config.h"
#include "LocalFileTerrainPageSource.h"

using namespace boost::filesystem;

using namespace NonUniformTerrain;

#define TERRAIN_PAGE_FILE_EXTENSION ".terrainpage"

LocalFileTerrainPageSource::LocalFileTerrainPageSource(const Config& config)
: TerrainPageSource()
{
	//m_pagesDir = config.GetSetting<path>("LocalFileTerrainPageSource.PagesDir");
	m_pagesDir = path("Data/Terrain/Bathymetry");
	debug << "LocalFileTerrainPageSource pages dir: " << m_pagesDir.directory_string().c_str() << endl; 
}

LocalFileTerrainPageSource::~LocalFileTerrainPageSource()
{
}

void LocalFileTerrainPageSource::LoadPage(unsigned int id, const TerrainParams& params)
{
	TerrainPageData* pPageData = LoadTerrainPageFromFile(id);
	OnPageDataLoaded(pPageData);
}

TerrainPageData* LocalFileTerrainPageSource::LoadTerrainPageFromFile(unsigned int id)
{
	TerrainPageData* pPageData = new TerrainPageData(id);
	String fileName = GetTerrainPageFileName(id);
	//debug << "Loading terrain page from " << fileName << endl;

	path filePath = m_pagesDir / fileName;

	// abort if file does not exist
	if (!exists(filePath))
	{
		//debug << "  FAILED - file " << filePath.string().c_str() << " does not exist" << endl; 
		return pPageData;
	}

	boost::filesystem::ifstream file;
	file.open(filePath, std::ios::binary);

	short power;
	file.read((char*)&power, sizeof(short));
	if (file.fail())
	{
		//debug << "  FAILED - reading page power" << endl; 
		goto file_read_error;
	}

	pPageData->Create(power);
	file.read((char*)pPageData->HeightField, pPageData->NumHeightSamples * sizeof(float));

	if (file.fail())
	{
		//debug << "  FAILED - reading sample array" << endl; 
		goto file_read_error;
	}

	file.close();

	// success
	pPageData->Status = TerrainPageStatus_OK;

	return pPageData;

file_read_error:
	file.close();
	SAFE_DELETE_ARRAY(pPageData->HeightField);
	return pPageData;
}

String LocalFileTerrainPageSource::GetTerrainPageFileName(unsigned int id)
{
	short i = TerrainPage::IFromID(id);
	short j = TerrainPage::JFromID(id);
	
	std::ostringstream o;
	
	o << ((i < 0) ? '-' : '+');

	o << std::setw(4) << std::setfill('0') 
		<< std::abs(i);
		
	//o << ','; 
	
	o << ((j < 0) ? '-' : '+');

	o << std::setw(4) << std::setfill('0')
		<< std::abs(j)  
		<< TERRAIN_PAGE_FILE_EXTENSION;

	return o.str();
}
