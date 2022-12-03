#include "../../StdAfx.h"
#include "FirstPerson.h"
#include "../World.h"

using namespace Ogre;

#define FIRST_PERSON_BOUNDING_SPHERE_RADIUS 2.0f
#define DEFAULT_FIRST_PERSON_SPEED 3000
#define FIRST_PERSON_MAX_YAW_RATE 10
#define FIRST_PERSON_MAX_PITCH_RATE 10


FirstPerson::FirstPerson(const Config& config, Ogre::Camera* pCamera)
: MovableThing("First Person"),
	m_pCamera(pCamera),
  m_pFollowTarget(NULL),
  m_following(false)
{
  m_pSceneNode->attachObject(m_pCamera);

  m_pCamera->setOrientation(Quaternion(Radian(Math::HALF_PI), Vector3::UNIT_X));

	m_boundingSphereRadius = FIRST_PERSON_BOUNDING_SPHERE_RADIUS;
	float speed = config.GetSetting<float>("FirstPersonSpeed", DEFAULT_FIRST_PERSON_SPEED);
	m_speeds = Vector3(speed, speed, speed);
	m_maxYawRate = FIRST_PERSON_MAX_YAW_RATE;
	m_maxPitchRate = FIRST_PERSON_MAX_PITCH_RATE;

  //// physics
  //ThingBulletMotionState* pMotionState = new ThingBulletMotionState(this);
  //btCollisionShape* pCollisionShape = new btSphereShape(m_boundingSphereRadius);
  //PhysicsBody = new btRigidBody(0, pMotionState, pCollisionShape);
  //PhysicsBody->setCollisionFlags(PhysicsBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
}

/*
const Vector3& FirstPerson::GetPosition() const 
{ 
  return m_pSceneNode->getPosition(); 
}

void FirstPerson::SetPosition(const Vector3& position) 
{ 
  m_pSceneNode->setPosition(position); 
}	

const Quaternion& FirstPerson::GetOrientation() const 
{ 
  return m_pSceneNode->getOrientation();
}

void FirstPerson::SetOrientation(const Quaternion& orientation) 
{ 
  m_pSceneNode->setOrientation(orientation); 
}

void FirstPerson::SetDirection(const Ogre::Vector3& dir) 
{ 
  m_pSceneNode->lookAt(WorldPosition + dir, Node::TS_WORLD, Thing::FORWARDS); 
}
*/
void FirstPerson::Move(float deltaTime)
{
  if (m_following)
    follow();

	if (m_localVelocity != Vector3::ZERO)
		m_pSceneNode->translate(m_speeds * m_localVelocity * deltaTime, Node::TS_LOCAL);

	if (m_worldVelocity != Vector3::ZERO)
		m_pSceneNode->translate(m_speeds * m_worldVelocity * deltaTime, Node::TS_WORLD);

	Radian yawAngle = m_yawAngle + Radian(deltaTime * m_yawRate * m_maxYawRate);
	if (yawAngle != Radian(0))
		yawWorldUp(yawAngle);

	Radian pitchAngle = m_pitchAngle + Radian(deltaTime * m_pitchRate * m_maxPitchRate);
	if (pitchAngle != Radian(0))
		pitch(pitchAngle);

	// calculate lat/long of new position 
	UpdateLocation();
}

void FirstPerson::yawWorldUp(const Radian& angle)
{
  m_pSceneNode->rotate(World::UP, angle, Node::TS_WORLD);
}

void FirstPerson::pitch(const Radian& angle)
{
  m_pSceneNode->rotate(Thing::RIGHT, angle, Node::TS_LOCAL);
}

void FirstPerson::startFollowing(Thing* pTarget)
{
  m_pFollowTarget = pTarget;
  m_following = true;
}

void FirstPerson::stopFollowing()
{
  m_following = false;
}

void FirstPerson::follow()
{
  Vector3 p = m_pFollowTarget->WorldPosition;
  WorldPosition = Vector3(p.x, p.y - 30.0f, 10.0f);
  //m_pCamera->lookAt(p);
}