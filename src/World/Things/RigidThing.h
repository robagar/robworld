#pragma once

#include "Thing.h"
#include "../../Physics/ThingBulletMotionState.h"
#include "../../Physics/RigidBodyProperties.h"

class RigidThing : public Thing
{
  btRigidBody* m_pRigidBody;

  ThingBulletMotionState* m_pMotionState;

public:
  RigidThing(const String& name, const String& meshName);
  virtual ~RigidThing();

  virtual void setPhysicsBody(btCollisionObject* pPhysicsBody) { m_pRigidBody = dynamic_cast<btRigidBody*>(pPhysicsBody); }
  virtual btCollisionObject* getPhysicsBody() { return m_pRigidBody; }

  void setRigidBody(const RigidBodyProperties& properties);
  btRigidBody* getRigidBody() { return m_pRigidBody; }
  __declspec(property(get=getRigidBody)) btRigidBody* RigidBody;

  virtual void setWorldPosition(const Vector3& position); 
  virtual void setLocalPosition(const Vector3& position); 

  virtual void setWorldOrientation(const Quaternion& orientation);
  virtual void setLocalOrientation(const Quaternion& orientation);

  virtual void setWorldVelocity(const Ogre::Vector3&);
  virtual Ogre::Vector3 getWorldVelocity() const;

  virtual void setLocalVelocity(const Ogre::Vector3&);
  virtual Ogre::Vector3 getLocalVelocity() const;

private:
  void updateBodyWorldTransform();

};