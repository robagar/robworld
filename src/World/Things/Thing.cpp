#include "../../StdAfx.h"
#include "Thing.h"
#include "../World.h"
#include "../../Physics/Physics.h"
#include "../../Physics/ThingBulletMotionState.h"
#include "ThingMaker.h"
#include "../../Physics/Float.h"

using namespace Ogre;
using namespace Geo;

const Vector3 Thing::FORWARDS = Vector3::UNIT_Y;
const Vector3 Thing::BACKWARDS = Vector3::NEGATIVE_UNIT_Y;
const Vector3 Thing::RIGHT = Vector3::UNIT_X;
const Vector3 Thing::LEFT = Vector3::NEGATIVE_UNIT_X;
const Vector3 Thing::UP = Vector3::UNIT_Z;
const Vector3 Thing::DOWN = Vector3::NEGATIVE_UNIT_Z;

SceneManager* Thing::s_pSceneManager;

Thing::Thing(const String& name)
: m_pWorld(NULL), 
	m_name(name),
  m_pParent(NULL),
	m_boundingSphereRadius(0.0f),
  m_pSceneNode(createSceneNode(name)),
	m_pRendererObject(NULL),
	m_showLabel(false)
{
}

Thing::Thing(const String& name, const String& meshName, const String& materialName)
: m_pWorld(NULL), 
	m_name(name),
  m_pParent(NULL),
	m_boundingSphereRadius(0.0f),
  m_pSceneNode(createSceneNode(name)),
	m_pRendererObject(NULL),
	m_showLabel(false)
{
  Entity* pEntity = s_pSceneManager->createEntity(name, meshName);
  pEntity->setMaterialName(materialName);
  setRendererObject(pEntity);
}

Thing::~Thing()
{
}

void Thing::setRendererObject(Ogre::MovableObject* pRendererObject) 
{ 
  if (m_pRendererObject)
    m_pSceneNode->detachObject(m_pRendererObject);

  m_pRendererObject = pRendererObject; 

  m_pSceneNode->attachObject(m_pRendererObject);
	m_pRendererObject->setUserAny(*this); 
}

// world position
const Vector3& Thing::getWorldPosition() const
{
  return m_pSceneNode->_getDerivedPosition();
}

void Thing::setWorldPosition(const Vector3& position) 
{ 
  if (m_pSceneNode->isInSceneGraph())
    m_pSceneNode->_setDerivedPosition(position);
  else
    setLocalPosition(position);
}

// local position
const Vector3& Thing::getLocalPosition() const
{
  return m_pSceneNode->getPosition();
}

void Thing::setLocalPosition(const Vector3& position) 
{ 
  m_pSceneNode->setPosition(position);
}



// world orientation
const Quaternion& Thing::getWorldOrientation() const
{
  return m_pSceneNode->_getDerivedOrientation();
}

void Thing::setWorldOrientation(const Quaternion& orientation)
{
  m_pSceneNode->_setDerivedOrientation(orientation);
}

// scale
void Thing::setScale(Real scale)
{
  m_pSceneNode->setScale(scale, scale, scale);
}

Real Thing::getScale() const
{
  return m_pSceneNode->getScale().x;
}

// local orientation
const Quaternion& Thing::getLocalOrientation() const
{
  return m_pSceneNode->getOrientation();
}

void Thing::setLocalOrientation(const Quaternion& orientation)
{
  m_pSceneNode->setOrientation(orientation);
}

void Thing::UpdateLocation()
{
	assert(m_pWorld);
	m_location = m_pWorld->Projection.CoordToLatLong(m_pWorld->PositionToMapCoord(WorldPosition));
}

void Thing::SetLocation(const Geo::GeoLocation& l)
{
	assert(m_pWorld);

	WorldPosition = m_pWorld->LatLongToPosition(l);
}

Vector3 Thing::getDirection() const
{
	return WorldOrientation * FORWARDS;
}

void Thing::setDirection(const Vector3& dir)
{
  m_pSceneNode->lookAt(WorldPosition + dir, Node::TS_WORLD, Thing::FORWARDS); 
}

Vector3 Thing::getUpDirection() const
{
	return WorldOrientation * UP;
}

void Thing::setUpDirection(const Vector3& dir)
{
  m_pSceneNode->lookAt(WorldPosition + dir, Node::TS_WORLD, Thing::UP); 
}


Ogre::Real Thing::getHeading() const
{
	Vector3 d = Direction;
	d.z = 0;
	float angle = Math::ACos(d.dotProduct(World::NORTH)).valueDegrees();
	return d.x >= 0 ? angle : 360 - angle;
}

void Thing::setHeading(const Real& heading)
{
  WorldOrientation = Quaternion(Radian(Degree(-heading)), World::UP);
}

Thing* Thing::createChild(const String& name, 
                          const String& meshName, 
                          const Ogre::Vector3& position, 
                          const Ogre::Quaternion& orientation)
{
  Thing* pChild = new Thing(name, meshName);
  attachChild(pChild);
  pChild->setLocalPosition(position);
  pChild->setLocalOrientation(orientation);
  return pChild;
}

bool Thing::isChild()
{
  return m_pParent != NULL;
}

void Thing::attachChild(Thing* pChild)
{
  assert(pChild->m_pParent == NULL);

  pChild->m_pParent = this;
  m_children.push_back(pChild);

  if (!pChild->PhysicsBody)
    m_pSceneNode->addChild(pChild->SceneNode);
}

void Thing::throwThing(Thing* pThing, Real speed)
{
  pThing->WorldPosition = WorldPosition + Direction * 3.0;
  pThing->WorldVelocity = Direction * speed;
  m_pWorld->AddThing(pThing);
}

String Thing::getDebugText()
{
  std::ostringstream o;

  o << "Position:    " << WorldPosition << endl;
  o << "Orientation: " << WorldOrientation << endl;
  o << "Direction:   " << Direction << endl;
  o << "Heading:     " << Heading << endl; 
  //o << "World Velocity:     " << WorldVelocity << endl; 
  //o << "Local Velocity:     " << LocalVelocity << endl; 

  if (isBouyant())
  {
    o << "Floats:" << endl;
    foreach(Float* pFloat, Floats)
      o << "  " << pFloat->getDebugText() << endl;
  }

  return o.str();
}