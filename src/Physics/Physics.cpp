#include "../StdAfx.h"
#include "../World/Things/Thing.h"
#include "../World/Things/RigidThing.h"
#include "../World/Things/SoftThing.h"
#include "../World/Ocean/Ocean.h"
#include "../World/World.h"
#include "Physics.h"
#include "Float.h"

using namespace Ogre;

Physics* Physics::s_pInstance  = NULL;

Physics::Physics()
  : m_pOcean(NULL),
    m_maxSubSteps(7)
{
  m_pBroadphase = new btDbvtBroadphase();

  //m_pCollisionConfig = new btDefaultCollisionConfiguration();
  m_pCollisionConfig = new btSoftBodyRigidBodyCollisionConfiguration();


  m_pCollisionDispatcher = new btCollisionDispatcher(m_pCollisionConfig);
  m_pSolver = new btSequentialImpulseConstraintSolver();
  
  //m_pDynamicsWorld = new btDiscreteDynamicsWorld(m_pCollisionDispatcher, m_pBroadphase, m_pSolver, m_pCollisionConfig);
  m_pDynamicsWorld = new btSoftRigidDynamicsWorld(m_pCollisionDispatcher, m_pBroadphase, m_pSolver, m_pCollisionConfig);

  m_pDynamicsWorld->setGravity(ogre2bullet(World::GRAVITY));

  initSoftBodyWorldInfo();
}

Physics::~Physics()
{
  delete(m_pDynamicsWorld);
  delete(m_pSolver);
  delete(m_pCollisionDispatcher);
  delete(m_pCollisionConfig);
  delete(m_pBroadphase);
}

Physics* Physics::create()
{
  s_pInstance = new Physics();
  return s_pInstance;
}

void Physics::initSoftBodyWorldInfo()
{
  m_softBodyWorldInfo.m_broadphase = m_pBroadphase;
  m_softBodyWorldInfo.m_dispatcher = m_pCollisionDispatcher;
  m_softBodyWorldInfo.m_gravity = ogre2bullet(World::GRAVITY);

  // At 20 °C and 101.325 kPa, dry air has a density of 1.2041 kg/m3.
  m_softBodyWorldInfo.air_density = 1.2041f;

  // disable water
  m_softBodyWorldInfo.water_density = 0;
  m_softBodyWorldInfo.water_normal = btVector3(0,0,0);
  m_softBodyWorldInfo.water_offset = 0;

  m_softBodyWorldInfo.m_sparsesdf.Initialize();


}

void Physics::addThing(Thing* pThing)
{
  // has rigid body?
  RigidThing* pRigidThing = dynamic_cast<RigidThing*>(pThing);
  if (pRigidThing)
    addRigidThing(pRigidThing);


  // has soft body?
  SoftThing* pSoftThing = dynamic_cast<SoftThing*>(pThing);
  if (pSoftThing)
    addSoftThing(pSoftThing);
}

void Physics::addRigidThing(RigidThing* pRigidThing)
{
  m_rigidThings.push_back(pRigidThing);

  addRigidBody(pRigidThing->RigidBody);

  if (pRigidThing->isBouyant())
    addBouyantThing(pRigidThing);

  //// temp disable gravity for children
  //if (pRigidThing->isChild())
  //  pRigidThing->RigidBody->setGravity(btVector3(0,0,0));
}

void Physics::addSoftThing(SoftThing* pSoftThing)
{
  addSoftBody(pSoftThing->SoftBody);
  m_softThings.push_back(pSoftThing);
}

void Physics::update(Real deltaTime)
{
  m_deltaTime = deltaTime;
  applyBouyancy();

  foreach(RigidThing* pThing, m_rigidThings)
    pThing->updatePhysics(m_deltaTime);

  foreach(SoftThing* pThing, m_softThings)
    pThing->updatePhysics(m_deltaTime);


  m_pDynamicsWorld->stepSimulation(deltaTime, m_maxSubSteps);

  updateSoftThings();
}

void Physics::applyBouyancy()
{
  if (!m_pOcean)
    return;

  foreach(RigidThing* pThing, m_bouyantThings)
  {
    foreach(Float* pFloat, pThing->Floats)
    {
      pFloat->applyBouyancyForce(m_pOcean, pThing);
    }
  }
}

btSoftBody* Physics::createSoftBody(const Geometry* pGeometry, const Real mass)
{
  btSoftBody* pSoftBody = btSoftBodyHelpers::CreateFromTriMesh(
      s_pInstance->m_softBodyWorldInfo, 
      (const btScalar*) &pGeometry->getVertexPositions()[0], 
      (const int*)&pGeometry->Indices[0],
      pGeometry->NumTriangles);

  // damping coeff (default 0.0)
  //pSoftBody->m_cfg.kDP = 0.001f;

  // dynamic friction coeff
  //pSoftBody->m_cfg.kDF = 0.5f;

  // pose matching coeff
  //pSoftBody->m_cfg.kMT = 0.01f;

  // pressure coeff
  //pSoftBody->m_cfg.kPR = 2500;

  
  // rigid contacts hardness
  //pSoftBody->m_cfg.kCHR = 1;

  // kinetic contacts hardness
  //pSoftBody->m_cfg.kKHR = 0.8f;

  // soft contacts hardness
  //pSoftBody->m_cfg.kSHR = 1;

  // anchor hardness [0-1], default 0.7
  pSoftBody->m_cfg.kAHR = 0.4f;

  // positions solver iterations
  //pSoftBody->m_cfg.piterations = 2;


  // linear stiffness coeff [0-1]
  //pSoftBody->m_materials[0]->m_kLST = 1.f;

  // area/angular stiffness coeff [0-1]
  //pSoftBody->m_materials[0]->m_kAST = 0.1f;

  // volume stiffness coeff [0-1]
  //pSoftBody->m_materials[0]->m_kVST = 0.5f;


  btSoftBody::Material* pMaterial = pSoftBody->appendMaterial();
  pMaterial->m_kLST = 0.9f;


  pSoftBody->setTotalMass(mass, false);

//  pSoftBody->randomizeConstraints();
  
  pSoftBody->generateBendingConstraints(2, pMaterial);

  //pSoftBody->generateClusters(0);

  pSoftBody->setPose(false, true);

  return pSoftBody;
}

void Physics::updateSoftThings()
{
  foreach(SoftThing* pThing, m_softThings)
    pThing->updateSoftBodyGeometry();
}