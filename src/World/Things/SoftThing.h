#pragma once

#include "Thing.h"

class SoftThing : public Thing
{
protected:
  btSoftBody* m_pSoftBody;

public:
  SoftThing(const String& name);

  virtual void setPhysicsBody(btCollisionObject* pPhysicsBody) { m_pSoftBody = dynamic_cast<btSoftBody*>(pPhysicsBody); }
  virtual btCollisionObject* getPhysicsBody() { return m_pSoftBody; }

  btSoftBody* getSoftBody() { return m_pSoftBody; }
  __declspec(property(get=getSoftBody)) btSoftBody* SoftBody;

  Ogre::uint32 getNumSoftBodyNodes() { return m_pSoftBody->m_nodes.size(); }

  void addForce(const Ogre::Vector3& force);

  virtual void updateSoftBodyGeometry();
};