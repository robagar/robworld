#include "../StdAfx.h"
#include "../World/Things/RigidThing.h"
#include "../World/Ocean/Ocean.h"
#include "Float.h"
#include "../World/World.h"
#include "Physics.h"

Float::Float(const String& name, Real length, Real volume, Vector3 localPosition)
  : m_name(name),
    m_length(length),
    m_volume(volume),
    m_localPosition(localPosition),
    m_force(Vector3::ZERO)
{
}

void Float::applyBouyancyForce(Ocean* pOcean, RigidThing* pThing)
{
  // get bottom of float world position
  m_worldPosition = pThing->SceneNode->convertLocalToWorldPosition(m_localPosition);

  //debug << "float z: " << p.z << endl;

  // underwater?
  Real depth = pOcean->positionDepth(m_worldPosition) + m_length / 2.0f;
  if (depth > 0.0f) 
  {
    Vector3 maxForce = m_volume /* * pOcean->Density */ * (-World::GRAVITY);

    Real f = depth > m_length ?  1.0f : depth / m_length;

    // TODO - proper drag
    //pThing->RigidBody->setDamping(0.99f, 0.99f); 
    
    m_force = f * maxForce;

    //debug << "force: " << force.z << endl;

    Vector3 relativePosition = m_worldPosition - pThing->WorldPosition;
    pThing->RigidBody->applyForce(ogre2bullet(m_force), ogre2bullet(relativePosition));
    pThing->RigidBody->activate();
  }
}

String Float::getDebugText()
{
  std::ostringstream o;
  o << m_name << ": " << m_worldPosition << ", " << m_force.z;
  return o.str();
}