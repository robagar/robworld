#include "stdafx.h"
#include "Boom.h"
#include "Boat.h"
#include "Physics.h"

using namespace Ogre;

Boom::Boom(const string& name, Ogre::Real mass)
  : RigidThing(name, "Boom.mesh"),
    m_pHinge(NULL),
    m_angle(0),
    m_minMaxAngle(pi/100.0f),
    m_maxMaxAngle(3.0f),
    m_maxAngleSpeed(0),
    m_maxAngle(3.0f)
{
  setRigidBody(RigidBodyProperties(new btCylinderShape(btVector3(0.07f, 2.1f, 0.07f)), mass));
}

void Boom::attachToBoat(Boat* pBoat)
{
  pBoat->attachChild(this);
  WorldOrientation = pBoat->WorldOrientation;
  LocalPosition = Vector3(0, -1.5f, 0.8f);

  m_pHinge = new btHingeConstraint(*pBoat->RigidBody, 
                                    *RigidBody, 
                                    btVector3(0.0f, 1.0f, 0.8f),
                                    btVector3(0.0f, 2.3f, 0.0f),
                                    btVector3(0, 0, 1.0f),
                                    btVector3(0, 0, 1.0f));

  Real m = m_maxAngle.valueRadians();
  m_pHinge->setLimit(-m, +m);
  
  Physics::instance()->m_pDynamicsWorld->addConstraint(m_pHinge, true);
}

Radian Boom::getAngle() const
{
  return Radian(m_pHinge->getHingeAngle());
}

void Boom::setAngle(Radian angle)
{
  // TODO
}

void Boom::setMaxAngleSpeed(const Real speed)
{
  m_maxAngleSpeed = speed;
}

void Boom::setMaxAngle(const Radian& maxAngle)
{
  m_maxAngle = clamp(maxAngle, m_minMaxAngle, m_maxMaxAngle);
}

void Boom::updatePhysics(Real deltaTime)
{
  setMaxAngle(m_maxAngle + Radian(deltaTime * m_maxAngleSpeed));

  Real m = m_maxAngle.valueRadians();
  m_pHinge->setLimit(-m, +m);

}

