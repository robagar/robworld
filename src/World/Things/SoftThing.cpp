#include "../../StdAfx.h"
#include "../../Physics/Physics.h"
#include "SoftThing.h"

using namespace Ogre;

SoftThing::SoftThing(const String& name)
  : Thing(name),
    m_pSoftBody(NULL)
{
}

void SoftThing::addForce(const Vector3& force)
{
  // force to apply to each node
  btVector3 f = ogre2bullet(force / (Real)getNumSoftBodyNodes());
  m_pSoftBody->addForce(f);
}

void SoftThing::updateSoftBodyGeometry()
{
  //for (int i=0; i<m_pSoftBody->m_nodes.size(); ++i)
  //{
  //  btSoftBody::Node& node = m_pSoftBody->m_nodes[i];
  //  m_pGeometry->setVertex(i, bullet2ogre(node.m_x), bullet2ogre(node.m_n));
  //}

  //m_pGeometry->updateGeometry();
}

