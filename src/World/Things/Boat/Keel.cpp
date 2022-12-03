#include "stdafx.h"
#include "Keel.h"
#include "Physics.h"

using namespace Ogre;

Keel::Keel(const string& name, Ogre::Real mass)
  : RigidThing(name, "Keel.mesh")
{
  setRigidBody(RigidBodyProperties(new btBoxShape(btVector3(0.06f, 0.6f, 0.6f)), mass));
}

void Keel::updatePhysics(Real deltaTime)
{
  Vector3 v = LocalVelocity;

//  btVector3 f(-(v.x * v.x) * 50.0f, -(v.y * v.y) * 0.5f, -(v.z * v.z) * 5.0f );
  btVector3 f(-v.x * 50.0f, -v.y * 0.5f, -v.z * 5.0f );
  btMatrix3x3 m = RigidBody->getWorldTransform().getBasis();

  RigidBody->applyCentralForce(m * f);
}