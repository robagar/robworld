#include "../StdAfx.h"
#include "RigidBodyProperties.h"

RigidBodyProperties::RigidBodyProperties(btCollisionShape* _pShape, btScalar _mass)
  : pShape(_pShape),
    mass(_mass),
    inertia(0, 0, 0),
    rigidBodyConstructionInfo(mass, NULL, pShape)
{
  restitution = rigidBodyConstructionInfo.m_restitution;
  friction = rigidBodyConstructionInfo.m_friction;
}


btRigidBody::btRigidBodyConstructionInfo& RigidBodyProperties::getRigidBodyConstructionInfo(btMotionState* pMotionState) const 
{
  rigidBodyConstructionInfo.m_mass = mass;
  rigidBodyConstructionInfo.m_restitution = restitution;
  rigidBodyConstructionInfo.m_friction = friction;

  pShape->calculateLocalInertia(mass, inertia);
  rigidBodyConstructionInfo.m_localInertia = inertia;

  rigidBodyConstructionInfo.m_motionState = pMotionState;

  return rigidBodyConstructionInfo;
}
