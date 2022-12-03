#pragma once

struct RigidBodyProperties
{
  btCollisionShape* pShape;
  btScalar mass;
  btScalar restitution;
  btScalar friction;

private:
  mutable btVector3 inertia;
  mutable btRigidBody::btRigidBodyConstructionInfo rigidBodyConstructionInfo;

public:
  RigidBodyProperties(btCollisionShape* _pShape, btScalar _mass);

  btRigidBody::btRigidBodyConstructionInfo& getRigidBodyConstructionInfo(btMotionState* pMotionState) const;
};