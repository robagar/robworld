#include "../../StdAfx.h"
#include "RigidThing.h"
#include "../../Physics/Physics.h"

RigidThing::RigidThing(const String& name, 
                       const String& meshName)
  : Thing(name, meshName),
    m_pRigidBody(NULL)
{
  m_pMotionState = new ThingBulletMotionState(this);
}

RigidThing::~RigidThing()
{
  delete m_pMotionState;
  delete m_pRigidBody;
}

void RigidThing::setRigidBody(const RigidBodyProperties& properties)
{
  m_pRigidBody = new btRigidBody(properties.getRigidBodyConstructionInfo(m_pMotionState));
}

void RigidThing::updateBodyWorldTransform()
{
  if (m_pRigidBody)
    m_pRigidBody->setCenterOfMassTransform(btTransform(ogre2bullet(WorldOrientation), ogre2bullet(WorldPosition)));
}


void RigidThing::setWorldPosition(const Vector3& position) 
{ 
  m_pSceneNode->setPosition(position);
  updateBodyWorldTransform();
}

void RigidThing::setLocalPosition(const Vector3& position) 
{
  setWorldPosition(m_pParent->WorldPosition + position);
  updateBodyWorldTransform();
}

void RigidThing::setWorldOrientation(const Quaternion& orientation)
{
  m_pSceneNode->setOrientation(orientation);
  updateBodyWorldTransform();
}

void RigidThing::setLocalOrientation(const Quaternion& orientation)
{
//  Thing::setLocalOrientation(orientation);
//  updateBodyWorldTransform();
}

void RigidThing::setWorldVelocity(const Vector3& velocity)
{
  m_pRigidBody->setLinearVelocity(ogre2bullet(velocity));
}

Ogre::Vector3 RigidThing::getWorldVelocity() const
{
  return bullet2ogre(m_pRigidBody->getLinearVelocity());
}

void RigidThing::setLocalVelocity(const Vector3& velocity)
{
  // TODO
  //m_pRigidBody->setLinearVelocity(ogre2bullet(velocity));
}

Ogre::Vector3 RigidThing::getLocalVelocity() const
{
  //btQuaternion q = m_pRigidBody->getOrientation().inverse();

  btMatrix3x3 m = m_pRigidBody->getWorldTransform().getBasis().inverse();
  btVector3 v = m_pRigidBody->getLinearVelocity();

  return bullet2ogre(m * v);
}