#pragma once

#include "RigidThing.h"

class Keel : public RigidThing
{
public:
  Keel(const string& name, Ogre::Real mass);

  virtual void updatePhysics(Ogre::Real deltaTime);
};