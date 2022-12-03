#include "../../StdAfx.h"
#include "../../Misc/Config.h"
#include "PerlinNoiseTerrainPageSource.h"
#include <noise.h>

using namespace NonUniformTerrain;

PerlinNoiseTerrainPageSource::PerlinNoiseTerrainPageSource(const Config& config)
: TerrainPageSource()
{
}

PerlinNoiseTerrainPageSource::~PerlinNoiseTerrainPageSource()
{
}

void PerlinNoiseTerrainPageSource::LoadPage(unsigned int id, const TerrainParams& params)
{
	TerrainPageData* pPageData = new TerrainPageData(id);
  pPageData->Create(10);

  noise::module::Perlin perlin;

  float x0 = params.GetPageX(TerrainPage::IFromID(id));
  float y0 = params.GetPageY(TerrainPage::JFromID(id));

  short n = pPageData->Size;
  float dx = params.PageWidth / n; 
  float dy = params.PageWidth / n; 

  float z0 = -25.0f;

  Vector3 baseScale(0.0005f, 0.0005f, 100.f);
  Vector3 scale(0.01f, 0.01f, 4.f);

  for(int j=0; j<n; ++j)
  {
    for(int i=0; i<n; ++i)
    {
      float x = x0 + i * dx;
      float y = y0 + j * dy;

      float b = baseScale.z * (float)perlin.GetValue(x * baseScale.x, y * baseScale.y, 1000.0);
      float h = scale.z * (float)perlin.GetValue(x * scale.x, y * scale.y, 0);

      pPageData->SetHeightAt(i, j, z0 + b + h);
    }
  }

  pPageData->Status = TerrainPageStatus_OK;
	OnPageDataLoaded(pPageData);
}


