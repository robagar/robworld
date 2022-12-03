#include "../StdAfx.h"

#include "../View/WorldView.h"
#include "Terrain/Terrain.h"
#include "Ocean/Ocean.h"
#include "Sky/Sky.h"
#include "World.h"
#include "WorldListener.h"
#include "Things/FirstPerson.h"
#include "../View/ObjectLabel.h"
#include "../Physics/Physics.h"
#include "Things/ThingMaker.h"
//#include "Things/Boat/Boat.h"
#include "../Geo/MercatorProjection.h"

using namespace Geo;

const Ogre::Vector3 World::UP    = Vector3::UNIT_Z;
const Ogre::Vector3 World::DOWN  = Vector3::NEGATIVE_UNIT_Z;
const Ogre::Vector3 World::NORTH = Vector3::UNIT_Y;
const Ogre::Vector3 World::EAST  = Vector3::UNIT_X;
const Ogre::Vector3 World::SOUTH = Vector3::NEGATIVE_UNIT_Y;
const Ogre::Vector3 World::WEST  = Vector3::NEGATIVE_UNIT_X;

const Ogre::Vector3 World::GRAVITY  = 9.8f * World::DOWN;

World* World::s_pInstance;

World::World(const Config& config, 
						 WorldListener* pListener, 
						 Ogre::SceneManager* pSceneManager,
						 Ogre::SceneNode* pSceneNode)
: m_config(config),
	m_pListener(pListener),
	m_pSceneManager(pSceneManager),
	m_pSceneNode(pSceneNode),
  m_time(0),
	m_pSun(NULL),
	m_pTerrain(NULL),
	m_pOcean(NULL),
  m_pPhysics(Physics::create())
{
  s_pInstance = this;

	m_pProjection = new MercatorProjection(MapCoordinate(0,0));
	Vector2 v = m_config.GetSetting<Vector2>("OriginLatLong");
	SetOriginLatLong(GeoLocation(v.x, v.y));

//	m_fogColor = ColourValue(0.93f, 0.86f, 0.76f);
  m_fogColor =  m_config.GetSetting<ColourValue>("FogColor");
	m_fogExponent = m_config.GetSetting<float>("FogExponent", 0.00001f);

  m_enableWind = m_config.GetSetting<bool>("EnableWind", false);
}

World::~World()
{
  delete m_pPhysics;

	delete m_pTerrain;
	delete m_pOcean;

	//delete m_pSun;
	delete m_pProjection;
}


MapCoordinate World::PositionToMapCoord(const Ogre::Vector3& v) const
{
	return MapCoordinate(v.x, -v.z); 
}

Ogre::Vector3 World::MapCoordToPosition(const Geo::MapCoordinate& c, bool onTerrainSurface, Real eyeHeight) const
{
  //Real altitude = (onTerrainSurface && m_pTerrain) ? m_pTerrain->GetHeight(c.east, c.north) : 0.0;

  //altitude += eyeHeight;

	return Ogre::Vector3((Real)c.east, (Real)c.north, eyeHeight);
}

Ogre::Vector3 World::LatLongToPosition(const Geo::GeoLocation& l,  bool onTerrainSurface, Real eyeHeight) const
{
	return MapCoordToPosition(m_pProjection->LatLongToCoord(l), onTerrainSurface, eyeHeight);
}

Geo::GeoLocation World::PositionToLatLong(const Ogre::Vector3& v) const
{
	return m_pProjection->CoordToLatLong(PositionToMapCoord(v));
}

void World::SetOriginLatLong(const GeoLocation& origin)
{
	// determine origin coords in projected space with origin at 0E,0N
	m_pProjection->SetOrigin(MapCoordinate(0,0));
	MapCoordinate c = m_pProjection->LatLongToCoord(origin);

	m_pProjection->SetOrigin(c);
}

void World::createContents()
{
  if (m_config.GetSetting<bool>("EnableTerrain"))
	  CreateTerrain();

  if (m_config.GetSetting<bool>("EnableOcean"))
	  CreateOcean();  

  if (m_config.GetSetting<bool>("EnableSky"))
	  CreateSky();
	
	CreateLights();

	CreateFirstPerson(WorldView::instance()->Camera);

  createThings();

  createInfoThings();
}

void World::CreateTerrain()
{
	m_pTerrain = new NonUniformTerrain::Terrain(this, m_pSceneNode->createChildSceneNode("terrain"), m_config);

  Vector3 startPosition = LatLongToPosition(m_config.GetSetting<GeoLocation>("StartLatLong"));
  m_pTerrain->Update(startPosition, Vector3::ZERO, m_config.GetSetting<float>("FarClipDistance"));
}

void World::CreateOcean()
{
	m_pOcean = new Ocean(WorldView::instance()->Camera, m_config);
  m_pPhysics->Ocean = m_pOcean;
}

void World::CreateSky()
{
	m_pSky = new Sky(m_pSceneManager, WorldView::instance()->Camera);
}

void World::CreateLights()
{
	// ambient
	m_ambientLightColor = Ogre::ColourValue(0.2f, 0.2f, 0.2f);
	m_pSceneManager->setAmbientLight(m_ambientLightColor); 

	// sun
	m_pSun = m_pSceneManager->createLight("the sun");
	m_pSun->setType(Ogre::Light::LT_DIRECTIONAL);
	m_pSun->setDiffuseColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f));
	m_pSun->setSpecularColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f));
	m_pSun->setDirection(0, 1.0f, -0.5f);

	//m_pAntiSun = m_pSceneManager->createLight("the anti sun");
	//m_pAntiSun->setType(Ogre::Light::LT_DIRECTIONAL);
	//m_pAntiSun->setDiffuseColour(0.4f * m_pSun->getDiffuseColour());
	////m_pAntiSun->setSpecularColour(0);
 // m_pAntiSun->setDirection(-m_pSun->getDirection());


	m_pSceneNode->attachObject(m_pSun);
	//m_pSceneNode->attachObject(m_pAntiSun);
}

FirstPerson* World::CreateFirstPerson(Ogre::Camera* pCamera)
{
  m_pFirstPerson = new ::FirstPerson(m_config, pCamera);
	AddThing(m_pFirstPerson);
  m_pFirstPerson->Location = m_config.GetSetting<GeoLocation>("StartLatLong");
  m_pFirstPerson->Heading = 225;
  //m_pFirstPerson->Direction = World::SOUTH;
  //m_pFirstPerson->Direction = World::EAST;
	return m_pFirstPerson;
}

void World::createThings()
{
  //m_pBoat = ThingMaker::boat();
  //AddThing(m_pBoat);

  ////m_pBoat->WorldPosition = Vector3(0.0, -20.0f, 0.0f);
  //m_pBoat->Heading = 90.0f;
  ////m_pBoat->Direction = World::EAST;


  ////m_pFirstPerson->startFollowing(m_pBoat);

  //for(int i=0l; i<30; ++i)
  //{
  //  for(int j=0; j<30; ++j)
  //  {
  //    Thing* pBall = ThingMaker::sphere();
  //    pBall->WorldPosition = Vector3(i, j, 10.0f);
  //    AddThing(pBall);
  //  }
  //}

  //AddThing(ThingMaker::sphere());
}

void World::createInfoThings()
{
//  AddThing(ThingMaker::worldAxes(100));
}


void World::AddThing(Thing* pThing)
{
	debug << "World: adding " << (pThing->IsMovable() ? "movable" : "static") << " thing named \"" << pThing->GetName().c_str() << "\"" << endl;

	pThing->World = this;

  if (!pThing->SceneNode->getParentSceneNode())
    m_pSceneNode->addChild(pThing->SceneNode);

	m_things.push_back(pThing);
	if (pThing->IsMovable())
		m_movableThings.push_back((MovableThing*)pThing);

  if (pThing->PhysicsBody) 
    m_pPhysics->addThing(pThing);

  // add children
  foreach(Thing*& pChild, pThing->Children)
    AddThing(pChild);

	OnThingAdded(pThing);
}


void World::OnThingAdded(Thing* pThing)
{
	if (m_pListener)
		m_pListener->ThingAddedToWorld(pThing);
}

void World::Update(Real deltaTime)
{
  m_time += deltaTime;
	m_deltaTime = deltaTime;

	ProcessCommands();
	MoveThings();

	const Vector3& firstPersonPosition = m_pFirstPerson->WorldPosition;

	if (m_pTerrain)
    m_pTerrain->Update(firstPersonPosition, /*m_pFirstPerson->GetVelocity()*/ Ogre::Vector3::ZERO, m_config.GetSetting<float>("FarClipDistance") /*m_pFirstPerson->GetFarClipDistance()*/);
	
	if (m_pOcean)
		m_pOcean->Update(m_time, m_deltaTime);

	if (m_pSky)
		m_pSky->update(m_time, m_deltaTime);

  m_pPhysics->update(m_deltaTime);
}

void World::ProcessCommands()
{
	while(!m_commandQueue.empty())
	{
		Command* pCommand = m_commandQueue.front();
		m_commandQueue.pop();

		pCommand->Execute();
		
		delete pCommand;
	}
}

void World::MoveThings()
{
	foreach(MovableThing* pThing, m_movableThings)
	{
		pThing->Move(m_deltaTime);
	}


	ConstrainThingsAboveTerrain();
}

void World::ConstrainThingsAboveTerrain()
{
  if (!m_pTerrain)
    return;

	foreach(MovableThing* pThing, m_movableThings)
	{
		Vector3 v = pThing->getWorldPosition();
		// get terrain height directly below
		float minAltitude = m_pTerrain->GetHeight(v.x, v.y) + pThing->GetBoundingSphereRadius(); 
		if (v.z < minAltitude)
		{
			v.z = minAltitude;
			pThing->setWorldPosition(v);
		}
	}
}

void World::TerrainResized(const AxisAlignedBox& terrainBoundingBox)
{
	m_boundingBox = terrainBoundingBox; 

	// allow space for things above terrain
	m_boundingBox.setMaximumZ(m_boundingBox.getMaximum().z + 1000.0f);
}

void World::TerrainPageAdded(NonUniformTerrain::TerrainPage* pPage)
{
  m_pPhysics->addRigidBody(pPage->PhysicsBody);
}

void World::TerrainPageRemoved(NonUniformTerrain::TerrainPage* pPage)
{
  m_pPhysics->removeRigidBody(pPage->PhysicsBody);
}

float World::GetTerrainHeight(float x, float y) const
{
	return m_pTerrain->GetHeight(x, y);
}

String World::GetDebugText()
{
	std::ostringstream o;

  o << "Me: " << endl << m_pFirstPerson->getDebugText() << endl;

	/*
	const NonUniformTerrain::TerrainStats& stats = m_pTerrain->GetStats();
	o << "terrain pages: " << stats.Pages << endl;
	o << "page loads: " << stats.PageLoads << endl;
	o << "page unloads: " << stats.PageUnloads << endl;
	o << "index data requests: " << stats.TileIndexDataRequests << " (" << (stats.TileIndexDataRequests - stats.TileIndexDataRequestCacheHits) << " cache misses)" << endl;
	*/

  
  //if (m_pOcean)
  //  o << m_pOcean->getDebugText();

  //o << "Boat: " << endl << m_pBoat->getDebugText(); 

	return o.str();
}

Vector3 World::getWindVelocity(const Vector3& position) const
{
  if (!m_enableWind)
    return Vector3::ZERO;

  // TODO

  const Real oneKnotMetersPerSecond = 0.51444444f;
  return Vector3(-10.0f * oneKnotMetersPerSecond, 0, 0);
}

