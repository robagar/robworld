#pragma once

#include "BulletUtil.h"

class Ocean;
class Thing;
class RigidThing;
class SoftThing;
class Geometry;

class Physics
{
public:
  static Physics* s_pInstance;

  btBroadphaseInterface* m_pBroadphase;
  btDefaultCollisionConfiguration* m_pCollisionConfig;
  btCollisionDispatcher* m_pCollisionDispatcher;
  btSequentialImpulseConstraintSolver* m_pSolver;
  
  //btDiscreteDynamicsWorld* m_pDynamicsWorld;
  btSoftRigidDynamicsWorld* m_pDynamicsWorld;

  btSoftBodyWorldInfo m_softBodyWorldInfo;

  int m_maxSubSteps;

  std::vector<RigidThing*> m_rigidThings;
  std::vector<SoftThing*> m_softThings;

  Ocean* m_pOcean;
  std::vector<RigidThing*> m_bouyantThings;


  Ogre::Real m_deltaTime;

private:
  Physics();

public:
  ~Physics();

  static Physics* create();
  static Physics* instance() { return s_pInstance; }

  void setOcean(Ocean* pOcean) { m_pOcean = pOcean; }
  __declspec(property(put=setOcean)) Ocean* Ocean;

  void addRigidBody(btRigidBody* pRigidBody) { m_pDynamicsWorld->addRigidBody(pRigidBody); }
  void removeRigidBody(btRigidBody* pRigidBody) { m_pDynamicsWorld->removeRigidBody(pRigidBody); }
  void addSoftBody(btSoftBody* pSoftBody) { m_pDynamicsWorld->addSoftBody(pSoftBody); }

  void addThing(Thing* pThing);
  void addRigidThing(RigidThing* pThing);
  void addSoftThing(SoftThing* pThing);

  static btSoftBody* createSoftBody(const Geometry* pGeometry, const Real mass);


  void update(Ogre::Real deltaTime);

private:
  void initSoftBodyWorldInfo();

  void addBouyantThing(RigidThing* pThing) { m_bouyantThings.push_back(pThing); }

  void applyBouyancy();

  void updateSoftThings();



};