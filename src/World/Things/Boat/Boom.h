#pragma once

#include "RigidThing.h"

class Boat;

class Boom : public RigidThing
{
  Ogre::Radian m_angle;
  Ogre::Radian m_maxAngle;

  Ogre::Radian m_minMaxAngle;
  Ogre::Radian m_maxMaxAngle;

  Ogre::Real m_maxAngleSpeed;

  btHingeConstraint* m_pHinge;


public:
  Boom(const string& name, Ogre::Real mass);

  void attachToBoat(Boat* pBoat);

  Ogre::Radian getAngle() const;
  void setAngle(Ogre::Radian angle);
  __declspec(property(get=getAngle, put=setAngle)) Ogre::Radian Angle;

  void setMaxAngleSpeed(const Ogre::Real speed);
  void setMaxAngle(const Ogre::Radian& maxAngle);
  Ogre::Radian getMaxAngle() const { return m_maxAngle; }
  __declspec(property(get=getMaxAngle, put=setMaxAngle)) Ogre::Radian MaxAngle;

  virtual void updatePhysics(Real deltaTime);
};