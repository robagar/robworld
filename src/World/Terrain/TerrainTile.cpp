#include "../../StdAfx.h"
#include "Terrain.h"
#include "TerrainPage.h"
#include "TerrainTile.h"
#include "TerrainTileIndexDataManager.h"

using namespace NonUniformTerrain;

String TerrainTile::s_movableType = "geomipmap";

#define PAGE_WIDTH_PARAM_INDEX 0
#define PAGE_POSITION_PARAM_INDEX 1

TerrainTile::TerrainTile(TerrainPage* pPage, 
												 Ogre::SceneNode* pSceneNode,
												 short i, short j, 
												 short size, 
												 float width,
												 TerrainTileIndexDataManager* pTerrainTileIndexDataManager)
: m_pTerrain(pPage->Terrain),
	m_pPage(pPage),
	m_pSceneNode(pSceneNode),
	m_size(size),
	m_width(width),
	m_pTerrainTileIndexDataManager(pTerrainTileIndexDataManager),
	m_params(pPage->GetPower(), i, j)
{
  setCastShadows(false);

	m_qSouth = m_params.J * (m_size-1);
	m_qNorth = m_qSouth + m_size - 1;
	m_pWest = m_params.I * (m_size-1);
	m_pEast = m_pWest + m_size - 1;

	m_pSceneNode->attachObject(this);
	//m_pSceneNode->showBoundingBox(true);
	//m_pSceneNode->setVisible(false);

	m_highestAvailableLOD = m_pPage->GetBestLOD();
	m_lowestAvailableLOD = m_pPage->GetWorstLOD();

	// calculate bounds and center position
	CalculateBoundingBox();

	CalculateLODMaxDistances();

	// shader parameters
	setCustomParameter(PAGE_WIDTH_PARAM_INDEX, Vector4(m_pPage->Width));
	setCustomParameter(PAGE_POSITION_PARAM_INDEX, Vector4(m_pPage->WestX, m_pPage->SouthY, 0, 0));
}

TerrainTile::~TerrainTile()
{
}

void TerrainTile::CalculateBoundingBox()
{
	m_west = m_pPage->GetX(m_pWest);
	m_south = m_pPage->GetY(m_qSouth);

	// find min and max heights
	float minZ = FLOAT_MAX;
	float maxZ = FLOAT_MIN;

	for (short q=m_qSouth; q <= m_qNorth; ++q)
	{
		for (short p=m_pWest; p <= m_pEast; ++p)
		{
			float z = m_pPage->GetZ(p, q);
			if (z < minZ) minZ = z;
			if (z > maxZ) maxZ = z;
		}
	}

	// set bounding box
	m_boundingBox.setExtents(m_west, m_south, minZ, 
													 m_west + m_width, m_south + m_width, maxZ);

	// average min and max to find center
	m_center = Vector3(m_west + m_width / 2, 
										 m_south + m_width / 2,
										 (minZ + maxZ) / 2);

	// calculate bounding sphere radius
	m_boundingRadius = (m_center - m_boundingBox.getMinimum()).length();
}

void TerrainTile::CalculateLODMaxDistances()
{
	for(int lod = m_highestAvailableLOD; lod < m_lowestAvailableLOD; ++lod)
	{
		// TODO
		m_lodMaxDistances.push_back((lod + 1) * 10000.0f);
	}
}

short TerrainTile::GetLODForDistance(float d) const
{
	// TODO: take vertical exagerration into account

	short lod = m_highestAvailableLOD;
	for(std::vector<float>::const_iterator it = m_lodMaxDistances.begin(); 
			it != m_lodMaxDistances.end() && *it < d;
			++it)
		++lod;

	if (lod > m_lowestAvailableLOD)
		lod = m_lowestAvailableLOD;

	return lod;
}

// Ogre::Renderable overrides
const Ogre::MaterialPtr& TerrainTile::getMaterial() const
{
	return m_pPage->GetMaterial();
}


void TerrainTile::getWorldTransforms(Matrix4* xform) const
{
  *xform = mParentNode->_getFullTransform();
}

const Quaternion& TerrainTile::getWorldOrientation() const
{
  return mParentNode->_getDerivedOrientation();
}

const Vector3& TerrainTile::getWorldPosition() const
{
  return mParentNode->_getDerivedPosition();
}

Real TerrainTile::getSquaredViewDepth(const Ogre::Camera* pCamera) const
{
  return (m_center - pCamera->getDerivedPosition()).squaredLength();
}

const Ogre::LightList& TerrainTile::getLights() const
{
  getParentSceneNode()->getCreator()->_populateLightList(
      m_center, this->getBoundingRadius(), m_lights);
	return m_lights;
}

// Ogre::MovableObject overrides
const String& TerrainTile::getMovableType() const
{
	return s_movableType;
}

void TerrainTile::_updateRenderQueue(Ogre::RenderQueue* queue)
{
  queue->addRenderable(this, mRenderQueueID);
}

void TerrainTile::_notifyCurrentCamera(Ogre::Camera *pCamera)
{
	UpdateLOD(pCamera->getDerivedPosition());
	//UpdateLOD(Vector3(0, 1000, 0));

	m_pTerrain->HeightShader->SetCustomShaderParameters(this);
}

void TerrainTile::UpdateLOD(const Vector3& cameraPosition)
{
	// get distance from camera to center of bounding box
	float d = (cameraPosition - m_center).length();

	m_oldLOD = m_params.LOD;
	m_params.LOD = GetLODForDistance(d);

	assert(m_params.LOD >= m_highestAvailableLOD);
	assert(m_params.LOD <= m_lowestAvailableLOD);
}

bool TerrainTile::UpdateNeighbourLODs()
{
	bool neighbourLODChanged = false;
	TerrainTile* pNeighbour;

	// determine neighbours with a different LOD
	// North 
	pNeighbour = m_neighbours[North];
	if (pNeighbour == NULL)
	{
		// same as this tile, so no stitching  
		m_params.NorthLOD = m_params.LOD;
	}
	else
	{
		if (m_params.NorthLOD != pNeighbour->m_params.LOD)
		{
			neighbourLODChanged = true;
			m_params.NorthLOD = pNeighbour->m_params.LOD;
		}
	}

	// East 
	pNeighbour = m_neighbours[East];
	if (pNeighbour == NULL)
	{
		// same as this tile, so no stitching  
		m_params.EastLOD = m_params.LOD;
	}
	else
	{
		if (m_params.EastLOD != pNeighbour->m_params.LOD)
		{
			neighbourLODChanged = true;
			m_params.EastLOD = pNeighbour->m_params.LOD;
		}
	}

	// South 
	pNeighbour = m_neighbours[South];
	if (pNeighbour == NULL)
	{
		// same as this tile, so no stitching  
		m_params.SouthLOD = m_params.LOD;
	}
	else
	{
		if (m_params.SouthLOD != pNeighbour->m_params.LOD)
		{
			neighbourLODChanged = true;
			m_params.SouthLOD = pNeighbour->m_params.LOD;
		}
	}

	// West 
	pNeighbour = m_neighbours[West];
	if (pNeighbour == NULL)
	{
		// same as this tile, so no stitching  
		m_params.WestLOD = m_params.LOD;
	}
	else
	{
		if (m_params.WestLOD != pNeighbour->m_params.LOD)
		{
			neighbourLODChanged = true;
			m_params.WestLOD = pNeighbour->m_params.LOD;
		}
	}

	return neighbourLODChanged;
}


void TerrainTile::getRenderOperation(Ogre::RenderOperation& op)
{
//	debug << "TerrainTile::getRenderOperation, tile " << m_i << "," << m_j << endl;
//	debug.flush();
	
	bool neighbourLODChanged = UpdateNeighbourLODs();
	if (m_params.LOD != m_oldLOD 
			|| neighbourLODChanged)
		m_pIndexData = m_pTerrainTileIndexDataManager->GetIndexData(m_params);

	op.vertexData = m_pPage->GetVertexData();
	op.indexData = m_pIndexData;
	op.useIndexes = true;
	op.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;
}


