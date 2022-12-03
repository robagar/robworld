#include "stdafx.h"
#include "Globals.h"
#include "SoftThing.h"
#include "Sail.h"
#include "Boom.h"
#include "Boat.h"
#include "World.h"
#include "Physics.h"

using namespace Ogre;

Sail::Sail(const string& name, 
           const Ogre::Vector3& head, const Ogre::Vector3& tack, const Ogre::Vector3& clew)
  : SoftThing(name),
    m_pBoat(NULL),
    m_pBoom(NULL)
{
  m_pGeometry = new SubdividedTriangle(g_pSceneManager->createManualObject(name), "Sail", 6, head, tack, clew);
  m_pGeometry->DoubleSided = true;

  RendererObject = m_pGeometry->ManualObjectPtr;

  m_pSoftBody = Physics::createSoftBody(m_pGeometry, 5.0f);

}

Sail::~Sail()
{
  delete m_pGeometry;
  delete m_pSoftBody;
}

Vector3 Sail::getWorldVelocity() const
{ 
  return m_pBoat->Boom.WorldVelocity; 
}

Vector3 Sail::getNormal() const
{
  Vector3 n = m_pBoat->UpDirection.crossProduct(m_pBoom->Direction);
  n.normalise();
  return n;
}

void Sail::attachToBoat(Boat* pBoat, Boom* pBoom)
{
  m_pBoat = pBoat;
  m_pBoom = pBoom;

  m_pBoat->attachChild(this);

  // attach to mast (assumed fixed to boat)
  foreach(uint32 i, luffEdgeIndices())
    SoftBody->appendAnchor(i, m_pBoat->RigidBody);

  // attach to boom
  foreach(uint32 i, footEdgeIndices())
    SoftBody->appendAnchor(i, m_pBoom->RigidBody);

}

const std::vector<uint32> Sail::luffEdgeIndices() const
{
  return m_pGeometry->getEdgeIndices(SubdividedTriangle::C);
}

const std::vector<uint32> Sail::footEdgeIndices() const
{
  return m_pGeometry->getEdgeIndices(SubdividedTriangle::A);
}

const std::vector<uint32> Sail::leachEdgeIndices() const
{
  return m_pGeometry->getEdgeIndices(SubdividedTriangle::B);
}

void Sail::updateSoftBodyGeometry()
{
  for (int i=0; i<m_pSoftBody->m_nodes.size(); ++i)
  {
    btSoftBody::Node& node = m_pSoftBody->m_nodes[i];
    m_pGeometry->setVertex(i, bullet2ogre(node.m_x), bullet2ogre(node.m_n));
  }

  m_pGeometry->updateGeometry();
}

void Sail::blow(const Vector3& windVelocity)
{
  debug << "wind velocity: " << windVelocity << endl; 
  debug << "world velocity: " << WorldVelocity << endl; 
  Vector3 apparentWindVelocity = windVelocity - WorldVelocity;

  debug << "apparent wind: " << apparentWindVelocity.length() << endl;

  // TODO
  Real forceCoeff = 20.0f;

  Vector3 n = Normal;
  Real area = projectedArea(apparentWindVelocity.normalisedCopy());
//  debug << "sail projected area: " << area << endl;
  Vector3 force = forceCoeff * area * apparentWindVelocity.dotProduct(n) * n;

  debug << "force: " << force.length() << endl;
  debug << "force forwards: " << force.dotProduct(m_pBoat->Direction) << endl;

  addForce(force);

/*
  btVector3 w = ogre2bullet(windVelocity);
  for (int i=0; i<m_pSoftBody->m_nodes.size(); ++i)
  {
    btSoftBody::Node& node = m_pSoftBody->m_nodes[i];
    
    btScalar coeff = 0.1f; 
    btVector3 force = coeff * node.m_n * node.m_n.dot(w); 

    m_pSoftBody->addForce(force, i);
  }
*/
}

void Sail::updatePhysics(Real deltaTime)
{
  blow(World::instance()->getWindVelocity(WorldPosition));
}

Real Sail::projectedArea(const Ogre::Vector3& planeNormal)
{
  // treat sail as flat triangle

  // get corner world positions
  Vector3 head = m_pGeometry->getVertexPosition(0);
  Vector3 tack = m_pGeometry->getVertexPosition(1);
  Vector3 clew = m_pGeometry->getVertexPosition(2);

  // get luff & leach edges as vectors
  Vector3 luff = tack - head;
  Vector3 leach = clew - head;

  // project onto plane
  Plane plane(planeNormal, 0);
  Vector3 luffProj = plane.projectVector(luff);
  Vector3 leachProj = plane.projectVector(leach);

  // calculate area
  return 0.5f * Math::Abs(luffProj.crossProduct(leachProj).length());
}