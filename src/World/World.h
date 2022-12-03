#pragma once

#include "../Command/Commands.h"
#include "Things/Thing.h"
#include "Things/FirstPerson.h"
#include "Terrain/TerrainListener.h"
#include "../Geo/MapProjection.h"

namespace NonUniformTerrain
{
	class Terrain;
}

class Physics;
class Ocean;
class Sky;
class WorldListener;
class Thing;
class MovableThing;
class FirstPerson;
class Boat;

#define DEFAULT_EYE_HEIGHT 1.8

class World : public TerrainListener
{
  static World* s_pInstance;

	const Config& m_config;

	WorldListener* m_pListener;
	Ogre::SceneManager* m_pSceneManager;
	Ogre::SceneNode* m_pSceneNode;

  Physics* m_pPhysics;

	NonUniformTerrain::Terrain* m_pTerrain;
	Ocean* m_pOcean;

	Sky* m_pSky;

	Ogre::ColourValue m_ambientLightColor;
	Ogre::Light* m_pSun;
	Ogre::Light* m_pAntiSun;

	std::list<Thing*> m_things;
	std::list<MovableThing*> m_movableThings;

	std::queue<Command*> m_commandQueue;

	FirstPerson* m_pFirstPerson;

	Geo::MapProjection* m_pProjection;

	Ogre::AxisAlignedBox m_boundingBox;

	Ogre::ColourValue m_fogColor;
	float m_fogExponent;

	float m_verticalScale;

  Ogre::Real m_time;
	Ogre::Real m_deltaTime;

public:
  //Boat* m_pBoat;
  bool m_enableWind;

public:
	static const Vector3 UP;
	static const Vector3 DOWN;
	static const Vector3 NORTH;
	static const Vector3 EAST;
	static const Vector3 SOUTH;
	static const Vector3 WEST;

	static const Vector3 GRAVITY;

public:
	World(const Config& config, WorldListener* pListener, Ogre::SceneManager* pSceneManager, Ogre::SceneNode* pSceneNode);
	~World();

  static World* instance() { return s_pInstance; }

	void SetOriginLatLong(const Geo::GeoLocation& origin);

	float GetVerticalScale() const { return m_verticalScale; }
	__declspec(property(get=GetVerticalScale)) float VerticalScale;

	Vector3 GetIsotropicScale(float scale) { return scale * Vector3(1.0f, 1.0f, 1.0f / m_verticalScale); }

	const Ogre::AxisAlignedBox& GetBoundingBox() const { return m_boundingBox; }

	Ogre::SceneManager* GetSceneManager() const { return m_pSceneManager; }
	__declspec(property(get=GetSceneManager)) Ogre::SceneManager* SceneManager;

	Ogre::SceneNode* GetSceneNode() const { return m_pSceneNode; }
	__declspec(property(get=GetSceneNode)) Ogre::SceneNode* SceneNode;

  void createContents();
	void CreateTerrain();
	void CreateOcean();
	void CreateSky();
  void createThings();
  void createInfoThings();

	void CreateLights();
	const Ogre::ColourValue& GetAmbientLightColor() const { return m_ambientLightColor; }

	FirstPerson* CreateFirstPerson(Ogre::Camera* pCamera);
	FirstPerson* GetFirstPerson() const { return m_pFirstPerson; }
	__declspec(property(get=GetFirstPerson)) FirstPerson* FirstPerson;

	Geo::MapProjection& GetProjection() const { return *m_pProjection; }
	__declspec(property(get=GetProjection)) Geo::MapProjection& Projection;

	Geo::MapCoordinate PositionToMapCoord(const Ogre::Vector3&) const;
	Ogre::Vector3 MapCoordToPosition(const Geo::MapCoordinate&, bool onTerrainSurface=true, Real eyeHeight = DEFAULT_EYE_HEIGHT) const; 

	Geo::GeoLocation PositionToLatLong(const Ogre::Vector3&) const;
	Ogre::Vector3 LatLongToPosition(const Geo::GeoLocation&, bool onTerrainSurface=true, Real eyeHeight = DEFAULT_EYE_HEIGHT) const;

	void AddThing(Thing* pThing);
	void OnThingAdded(Thing* pThing);


	std::queue<Command*>& GetCommandQueue() { return m_commandQueue; }

	void Update(Ogre::Real deltaTime);
	void ProcessCommands();
	void MoveThings();

	void OnResized();

	NonUniformTerrain::Terrain* GetTerrain() { return m_pTerrain; }
	__declspec(property(get=GetTerrain)) NonUniformTerrain::Terrain* Terrain;
	float GetTerrainHeight(float x, float y) const;

	const Ogre::ColourValue& GetFogColor() const { return m_fogColor; }
	__declspec(property(get=GetFogColor)) ColourValue& FogColor;

	float GetFogExponent() const { return m_fogExponent; }
	__declspec(property(get=GetFogExponent)) float FogExponent;

	String GetDebugText();

	Ocean* GetOcean() { return m_pOcean; }

  Ogre::Vector3 getWindVelocity(const Ogre::Vector3& position) const;

	// TerrainListener
	void TerrainResized(const AxisAlignedBox& terrainBoundingBox);
  void TerrainPageAdded(NonUniformTerrain::TerrainPage* pPage);
  void TerrainPageRemoved(NonUniformTerrain::TerrainPage* pPage);

private:
	void ConstrainThingsAboveTerrain();
};

