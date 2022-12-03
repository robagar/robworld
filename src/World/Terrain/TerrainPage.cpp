#include "../../StdAfx.h"
#include "Terrain.h"
#include "TerrainPage.h"
#include "TerrainTile.h"

using namespace NonUniformTerrain;

#define ID_I_SHIFT 16
#define ID_J_MASK 0xffff

#define MAIN_BINDING 0

#define BEST_LOD 0

String TerrainPage::GetName(unsigned int id)
{
	return GetName(IFromID(id), JFromID(id));
}

String TerrainPage::GetName(short i, short j)
{
	std::ostringstream o;
	
	o << ((i < 0) ? '-' : '+');

	o << std::setw(4) << std::setfill('0') 
		<< abs(i);
		
	o << ((j < 0) ? '-' : '+');

	o << std::setw(4) << std::setfill('0')
		<< abs(j);  

	return o.str();
}

unsigned int TerrainPage::ID(short i, short j)
{
	return (unsigned short)i << ID_I_SHIFT | (unsigned short)j;
}

short TerrainPage::IFromID(unsigned int id)
{
	return (short)(id >> ID_I_SHIFT);
}

short TerrainPage::JFromID(unsigned int id)
{
	return (short)(id & ID_J_MASK);
}

TerrainPage::TerrainPage(NonUniformTerrain::Terrain* pTerrain,
												 const TerrainPageData& pageData)
: m_pTerrain(pTerrain),
	m_terrainParams(pTerrain->Params), 
	m_pSceneNode(NULL),
	m_i(pageData.I), 
	m_j(pageData.J),
	m_heightfield(NULL)
{
	//m_pSceneNode->showBoundingBox(true);

	m_name = GetName(m_i, m_j);

	m_power = pageData.Power;
	m_size = pageData.Size;
	m_tileSize = m_terrainParams.TileSize;
	m_sizeInTiles = m_terrainParams.PageSizeInTiles;

	// initialize bounding box
	// (z values will be calculated and filled in when the vertex buffer is filled)
	m_west = m_terrainParams.GetPageX(m_i);
	m_south = m_terrainParams.GetPageY(m_j);
	m_width = m_terrainParams.PageWidth;
	m_boundingBox.setExtents(m_west, m_south, 0, 
													 m_west + m_width, m_south + m_width, 0);

	m_tileWidth = m_width / m_sizeInTiles;
	m_bestLODPointSpacing = m_terrainParams.GetBestLODPointSpacing();

	Create(pageData);

	InitializeMaterial();

}

TerrainPage::~TerrainPage()
{
  delete m_pPhysicsBody;
  delete m_pMotionState;
  delete m_pCollisionShape;

	delete[] m_heightfield;

	foreach(TerrainTile* pTile, m_tiles)
		delete pTile;
}


float TerrainPage::GetX(short p)
{
	return m_west + (p * m_bestLODPointSpacing); 
}


float TerrainPage::GetY(short q)
{
	return m_south + (q * m_bestLODPointSpacing); 
}

float TerrainPage::GetHeight(float x, float y)
{
	return GetInterpolatedHeight(x, y, BEST_LOD);
}

float TerrainPage::GetInterpolatedHeight(float x, float y, short lod)
{
	short quadSize = 1 << lod;
	float quadWidth = quadSize * m_bestLODPointSpacing;

	// use coordinates relative to page northwest corner, 1 unit is width of a quad
	float xQuadSpace = (x - m_west) / quadWidth;
	float yQuadSpace = (y - m_south) / quadWidth;

	// get southwest corner of containing quad point indices
	short quadWest = (short)xQuadSpace * quadSize; 
	short quadSouth = (short)yQuadSpace * quadSize;

	// get coordinates relative to this quad
	float xQuadLocal = xQuadSpace - floor(xQuadSpace);
	float yQuadLocal = yQuadSpace - floor(yQuadSpace);

	// determine which triangle it's in
	Plane triPlane;
	if (xQuadLocal + yQuadLocal < 1.0f)
	{
		// southwest tri
		triPlane.redefine(Vector3(0, 0, GetZ(quadWest, quadSouth)), 
											Vector3(0, 1, GetZ(quadWest, (short)(quadSouth + quadSize))),
											Vector3(1, 0, GetZ((short)(quadWest + quadSize), quadSouth)));
	}
	else
	{
		// northeast tri
		triPlane.redefine(Vector3(0, 1, GetZ(quadWest, quadSouth + quadSize)), 
											Vector3(1, 1, GetZ((short)(quadWest + quadSize), quadSouth + quadSize)),
											Vector3(1, 0, GetZ((short)(quadWest + quadSize), quadSouth)));
	}

	// intersect vertical ray with plane
	Vector3 rayOrigin(xQuadLocal, 0, yQuadLocal);
	float nom = triPlane.normal.dotProduct(rayOrigin) + triPlane.d;
  float denom = triPlane.normal.dotProduct(Vector3::UNIT_Z);
	float z = -(nom/denom);

	return z;
}

void TerrainPage::Create(const TerrainPageData& pageData)
{
	// debug << "TerrainPage: creating page " << m_i << "," << m_j << endl; 

	// sanity check - good data?
	if (!pageData.IsGood())
		throw new std::exception("bad terrain page data");

	m_power = pageData.Power;
	//debug << "  power: " << m_power << endl;
	m_bestLOD = m_terrainParams.GetLOD(m_power);
	//debug << "  best LOD: " << m_bestLOD << endl;
	m_worstLOD = m_terrainParams.WorstLOD;
	//debug << "  worst LOD: " << m_worstLOD << endl;

	// make system memory copy of the heightfield
	m_heightfield = new float[pageData.NumHeightSamples];
	memcpy(m_heightfield, pageData.HeightField, pageData.NumHeightSamples * sizeof(float));
}

void TerrainPage::AddedToTerrain(Ogre::SceneNode* pSceneNode, const std::vector<TerrainPage*>& neighbours, TerrainTileIndexDataManager* pTerrainTileIndexDataManager)
{
	m_pSceneNode = pSceneNode;
	m_pTerrainTileIndexDataManager = pTerrainTileIndexDataManager;
	
	// initialize tiles
	CreateTiles();
	SetTileNeighbours();

	// link tiles across page boundaries
	SetInterPageTileNeighbours(neighbours);

	// adjust vertices along edges to match already loaded pages
	AdjustHeightfieldToMatchNeighbours(neighbours);

	// initialize vertex info and hardware vertex buffer
	InitVertexData();

  // create rigid body for collision detection
  initPhysics();
}

void TerrainPage::RemovedFromTerrain(const std::vector<TerrainPage*>& neighbours)
{
	m_pSceneNode = NULL;

	// break links between tiles across page boundaries
	UnsetInterPageTileNeighbours(neighbours);
}

void TerrainPage::AdjustHeightfieldToMatchNeighbours(const std::vector<TerrainPage*>& neighbours)
{
	TerrainPage* pNeighbour;
	short step = 1 << m_bestLOD;
	short zeroLODPageSize = m_terrainParams.ZeroLODPageSize;
	short end = zeroLODPageSize-1;

	// south
	pNeighbour = neighbours[South];
	if (pNeighbour != NULL)
	{
		for (short p=0; p<=end; p += step)
			SetZ(p, 0, pNeighbour->GetZ(p, end));
	}

	// east
	pNeighbour = neighbours[East];
	if (pNeighbour != NULL)
	{
		for (short q=0; q<=end; q+=step)
			SetZ(end, q, pNeighbour->GetZ(0, q));
	}

	// north
	pNeighbour = neighbours[North];
	if (pNeighbour != NULL)
	{
		for (short p=end; p>=0; p-=step)
			SetZ(p, end, pNeighbour->GetZ(p, 0));
	}

	// west
	pNeighbour = neighbours[West];
	if (pNeighbour != NULL)
	{
		for (short q=end; q>=0; q-=step)
			SetZ(0, q, pNeighbour->GetZ(end, q));
	}
}

void TerrainPage::InitVertexData()
{
	m_vertexBufferStride = m_size;

	// vertex source info
	m_vertexData.vertexStart = 0;
	m_vertexData.vertexCount = m_vertexBufferStride * m_vertexBufferStride;

  Ogre::VertexDeclaration* decl = m_vertexData.vertexDeclaration;
 
  size_t offset = 0;

	// positions
	decl->addElement(MAIN_BINDING, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
  offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

	if (m_terrainParams.VertexLightingEnabled)
	{
		// normals
		decl->addElement(MAIN_BINDING, offset, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
		offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
	}

  m_pVertexBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
			decl->getVertexSize(MAIN_BINDING),
			m_vertexData.vertexCount, 
			Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

	m_vertexData.vertexBufferBinding->setBinding(MAIN_BINDING, m_pVertexBuffer);

	FillVertexBuffer();
}


void TerrainPage::FillVertexBuffer()
{
  unsigned char* pBufferData = static_cast<unsigned char*>(m_pVertexBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD));

  Ogre::VertexDeclaration* decl = m_vertexData.vertexDeclaration;
 
	size_t vertexSize = m_pVertexBuffer->getVertexSize();

	// positions
	Vector3& boundingBoxMin = m_boundingBox.getMinimum(); 

	float minZ = FLOAT_MAX;
	float maxZ = FLOAT_MIN;

	unsigned char* pBase = pBufferData;
	const Ogre::VertexElement* positionElement = decl->findElementBySemantic(Ogre::VES_POSITION);
	short step = 1 << m_bestLOD; 
	short zeroLODPageSize = m_terrainParams.ZeroLODPageSize;
	for (short q=0; q<zeroLODPageSize; q += step)
	{
		float y = GetY(q);
		for (short p=0; p<zeroLODPageSize; p += step)
		{
			Vector3* pPosition;
			positionElement->baseVertexPointerToElement(pBase, (void**)&pPosition);

			float z = GetZ(p,q);

			pPosition->x = GetX(p);
			pPosition->y = y;
			pPosition->z = z;

			if (z < minZ)
				minZ = z;
			if (z > maxZ)
				maxZ = z; 

			pBase += vertexSize;
		}
	}

	// normals
  if (m_pTerrain->GetParams().VertexLightingEnabled)
	{
		const Ogre::VertexElement* normalElement = decl->findElementBySemantic(Ogre::VES_NORMAL);

		unsigned char* pBase = pBufferData;
		for (short q=0; q<m_size; ++q)
		{
			for (short p=0; p<m_size; ++p)
			{
				Vector3* pNormal;
				normalElement->baseVertexPointerToElement(pBase, (void**)&pNormal);
				GetNormal(p, q, *pNormal);
				pBase += vertexSize;
			}
		}
	}

	m_pVertexBuffer->unlock();

	// update bounding box min & max z
	m_boundingBox.setMinimumZ(minZ);
	m_boundingBox.setMaximumZ(maxZ);
}

void TerrainPage::CreateTiles()
{
	//debug << "  creating tiles..." << endl; 
	for (short j=0; j<m_sizeInTiles; ++j)
	{
		for (short i=0; i<m_sizeInTiles; ++i)
		{
			CreateTile(i,j);
		}
	}
}

void TerrainPage::CreateTile(short i, short j)
{
	//debug << "  creating tile " << i << "," << j << endl;

	// create scene node
	std::ostringstream o;
	o << "terrain page " << m_i << "," << m_j << " tile " << i << "," << j;
	Ogre::SceneNode* pSceneNode = m_pSceneNode->createChildSceneNode(o.str());

	TerrainTile* pTile = new TerrainTile(this, pSceneNode, i, j, m_tileSize, m_tileWidth, m_pTerrainTileIndexDataManager);
	m_tiles.push_back(pTile);
}

void TerrainPage::SetTileNeighbours()
{
	for (short j=0; j<m_sizeInTiles; ++j)
	{
		for (short i=0; i<m_sizeInTiles; ++i)
		{
			TerrainTile* pTile = GetTile(i,j);
			pTile->SetNeighbour(South, j != 0 ?               GetTile(i, j-1) : NULL);
			pTile->SetNeighbour(North, j != m_sizeInTiles-1 ? GetTile(i, j+1) : NULL);
			pTile->SetNeighbour(West,  i != 0 ?               GetTile(i-1, j) : NULL);
			pTile->SetNeighbour(East,  i != m_sizeInTiles-1 ? GetTile(i+1, j) : NULL);
		}
	}
}

void TerrainPage::SetInterPageTileNeighbours(const std::vector<TerrainPage*>& neighbours)
{
	TerrainPage* pNeighbour;

	// south
	pNeighbour = neighbours[South];
	if (pNeighbour != NULL)
	{
		assert(m_sizeInTiles == pNeighbour->m_sizeInTiles);
		for (short i=0; i<m_sizeInTiles; ++i)
		{
			TerrainTile* pTile = GetTile(i,0);
			TerrainTile* pNeighbourTile = pNeighbour->GetTile(i, m_sizeInTiles-1);
			pTile->SetNeighbour(South, pNeighbourTile);
			pNeighbourTile->SetNeighbour(North, pTile);
		}
	}

	// east
	pNeighbour = neighbours[East];
	if (pNeighbour != NULL)
	{
		assert(m_sizeInTiles == pNeighbour->m_sizeInTiles);
		for (short j=0; j<m_sizeInTiles; ++j)
		{
			TerrainTile* pTile = GetTile(m_sizeInTiles-1,j);
			TerrainTile* pNeighbourTile = pNeighbour->GetTile(0, j);
			pTile->SetNeighbour(East, pNeighbourTile);
			pNeighbourTile->SetNeighbour(West, pTile);
		}
	}

	// north
	pNeighbour = neighbours[North];
	if (pNeighbour != NULL)
	{
		assert(m_sizeInTiles == pNeighbour->m_sizeInTiles);
		for (short i=0; i<m_sizeInTiles; ++i)
		{
			TerrainTile* pTile = GetTile(i, m_sizeInTiles-1);
			TerrainTile* pNeighbourTile = pNeighbour->GetTile(i, 0);
			pTile->SetNeighbour(North, pNeighbourTile);
			pNeighbourTile->SetNeighbour(South, pTile);
		}
	}

	// west
	pNeighbour = neighbours[West];
	if (pNeighbour != NULL)
	{
		assert(m_sizeInTiles == pNeighbour->m_sizeInTiles);
		for (short j=0; j<m_sizeInTiles; ++j)
		{
			TerrainTile* pTile = GetTile(0,j);
			TerrainTile* pNeighbourTile = pNeighbour->GetTile(m_sizeInTiles-1, j);
			pTile->SetNeighbour(West, pNeighbourTile);
			pNeighbourTile->SetNeighbour(East, pTile);
		}
	}
}

void TerrainPage::UnsetInterPageTileNeighbours(const std::vector<TerrainPage*>& neighbours)
{
	TerrainPage* pNeighbour;

	// south
	pNeighbour = neighbours[South];
	if (pNeighbour != NULL)
	{
		assert(m_sizeInTiles == pNeighbour->m_sizeInTiles);
		for (short i=0; i<m_sizeInTiles; ++i)
		{
			TerrainTile* pNeighbourTile = pNeighbour->GetTile(i, m_sizeInTiles-1);
			pNeighbourTile->SetNeighbour(North, NULL);
		}
	}

	// east
	pNeighbour = neighbours[East];
	if (pNeighbour != NULL)
	{
		assert(m_sizeInTiles == pNeighbour->m_sizeInTiles);
		for (short j=0; j<m_sizeInTiles; ++j)
		{
			TerrainTile* pNeighbourTile = pNeighbour->GetTile(0, j);
			pNeighbourTile->SetNeighbour(West, NULL);
		}
	}

	// north
	pNeighbour = neighbours[North];
	if (pNeighbour != NULL)
	{
		assert(m_sizeInTiles == pNeighbour->m_sizeInTiles);
		for (short i=0; i<m_sizeInTiles; ++i)
		{
			TerrainTile* pNeighbourTile = pNeighbour->GetTile(i, 0);
			pNeighbourTile->SetNeighbour(South, NULL);
		}
	}

	// west
	pNeighbour = neighbours[West];
	if (pNeighbour != NULL)
	{
		assert(m_sizeInTiles == pNeighbour->m_sizeInTiles);
		for (short j=0; j<m_sizeInTiles; ++j)
		{
			TerrainTile* pNeighbourTile = pNeighbour->GetTile(m_sizeInTiles-1, j);
			pNeighbourTile->SetNeighbour(East, NULL);
		}
	}
}

void TerrainPage::GetNormal(short p, short q, Vector3& normal)
{
	Vector3 here = GetPoint(p, q);
	Vector3 vSouth = (q == 0) ? here - GetPoint(p, q+1) : GetPoint(p, q-1) - here;
	Vector3 vWest = (p == 0) ? here - GetPoint(p+1, q) : GetPoint(p-1, q) - here; 
	normal = vWest.crossProduct(vSouth);
  normal.normalise();
}

void TerrainPage::InitializeMaterial()
{
  //m_pMaterial = Ogre::MaterialManager::getSingleton().getByName("default");
  //return;
  /*
	String materialName = GetMaterialName();
	m_pMaterial = Ogre::MaterialManager::getSingleton().getByName(materialName);
	if (!m_pMaterial.isNull())
		return;

	Ogre::MaterialPtr pReferenceMaterial = Ogre::MaterialManager::getSingleton().getByName("terrain_material");
	assert (!pReferenceMaterial.isNull());
		
	pReferenceMaterial->load();

	// make copy of the material
	m_pMaterial = pReferenceMaterial->clone(materialName);

  Ogre::Pass* pPass = m_pMaterial->getTechnique(0)->getPass(0);

  pPass->setLightingEnabled(m_pTerrain->GetParams().VertexLightingEnabled);

	// set the height shader texture
	Ogre::TextureUnitState* pHeightShaderTextureUnitState = pPass->getTextureUnitState("height_shader");
	assert(pHeightShaderTextureUnitState);
	pHeightShaderTextureUnitState->setTextureName(m_pTerrain->HeightShader->TextureName);

	// set the lightmap texture name
	Ogre::TextureUnitState* pLightmapTextureUnitState = pPass->getTextureUnitState("lightmap");
  if (m_pTerrain->GetParams().LightMapEnabled)
  {
	  assert(pLightmapTextureUnitState);
	  pLightmapTextureUnitState->setTextureName(GetLightMapTextureName());
  }
  */

  
	String materialName = GetMaterialName();
	m_pMaterial = Ogre::MaterialManager::getSingleton().getByName(materialName);
	if (!m_pMaterial.isNull())
		return;

	Ogre::MaterialPtr pReferenceMaterial = Ogre::MaterialManager::getSingleton().getByName("terrain_with_ocean_depth_material");
	assert (!pReferenceMaterial.isNull());
		
	pReferenceMaterial->load();

	// make copy of the material
	m_pMaterial = pReferenceMaterial->clone(materialName);

  Ogre::Pass* pPass = m_pMaterial->getTechnique(0)->getPass(0);

	// set the height shader texture
	Ogre::TextureUnitState* pHeightShaderTextureUnitState = pPass->getTextureUnitState("height_with_ocean_depth");
	assert(pHeightShaderTextureUnitState);
	pHeightShaderTextureUnitState->setTextureName(m_pTerrain->HeightShader->TextureName);
  
  /*
  String materialName = GetMaterialName();
	m_pMaterial = Ogre::MaterialManager::getSingleton().getByName(materialName);
	if (!m_pMaterial.isNull())
		return;

	Ogre::MaterialPtr pReferenceMaterial = Ogre::MaterialManager::getSingleton().getByName("noise_test_material");
	assert (!pReferenceMaterial.isNull());
		
	pReferenceMaterial->load();

	// make copy of the material
	m_pMaterial = pReferenceMaterial->clone(materialName);
  */

}

String TerrainPage::GetMaterialName()
{
	std::ostringstream o;
	o << m_name << "_material";
	return o.str();
}

String TerrainPage::GetLightMapTextureName()
{
	std::ostringstream o;
	o << m_name << "_lightmap.png";
	return o.str();
}

void TerrainPage::initPhysics()
{
  btScalar minHeight = m_boundingBox.getMinimum().z;
  btScalar maxHeight = m_boundingBox.getMaximum().z;

  m_pCollisionShape = new btHeightfieldTerrainShape(m_size, m_size, 
                                                    m_heightfield,
                                                    1.0, // scale, not used for float data
                                                    minHeight, 
                                                    maxHeight,
                                                    2, // z is up,
                                                    PHY_FLOAT, 
                                                    false);

  // scale horizontally by the distance between grid points
  btScalar gridSpacing  = m_width / (m_size - 1);
  m_pCollisionShape->setLocalScaling(btVector3(gridSpacing, gridSpacing, 1.0));

  // set origin to middle of heightfield
  Vector3 c = m_boundingBox.getCenter();
  btTransform tr;
	tr.setIdentity();
	tr.setOrigin(btVector3(c.x, c.y, c.z));

  m_pMotionState = new btDefaultMotionState(tr);

  m_pPhysicsBody = new btRigidBody(0, m_pMotionState, m_pCollisionShape);
  m_pPhysicsBody->setRestitution(0.5);
}