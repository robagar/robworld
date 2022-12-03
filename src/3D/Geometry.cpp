#include "../StdAfx.h"
#include "Geometry.h"

using namespace Ogre;

Geometry::Geometry(Ogre::ManualObject* pManualObject, const Ogre::String& materialName)
  : m_pManualObject(pManualObject),
    m_materialName(materialName), 
    m_numTriangles(0),
    m_doubleSided(false)
{
}

uint32 Geometry::addVertex(const Ogre::Vector3& position, const Ogre::Vector3& normal, const Ogre::uint32 flags)
{
  Vertex v;
  v.position = position;
  v.normal = normal;
  v.flags = flags;

  m_vertices.push_back(v);

  m_pManualObject->position(position);
  m_pManualObject->normal(normal);

  return m_vertices.size() - 1;
}

uint32 Geometry::addVertexIfNew(const Ogre::Vector3& position, const Ogre::Vector3& normal, const Ogre::uint32 flags)
{
  for(uint32 i=0; i<m_vertices.size(); ++i)
  {
    Vertex& v = m_vertices[i];
    if (v.position.positionEquals(position)
        && v.normal.directionEquals(normal, Radian(0.01f))
        && v.flags == flags)
      return i;
  }

  return addVertex(position, normal, flags);
}

void Geometry::addTriangle(const Ogre::uint32 i, const Ogre::uint32 j, const Ogre::uint32 k)
{
  debug << "add tri " << i << ", " << j << ", " << k << endl;

  m_indices.push_back(i);
  m_indices.push_back(j);
  m_indices.push_back(k);

  m_numTriangles++;

  m_pManualObject->triangle(i, j, k);
}

void Geometry::updateGeometry()
{
  m_pManualObject->beginUpdate(0);

  uint32 n = m_vertices.size();

  for(unsigned int i=0; i<n; ++i)
  {
    Vertex& v = m_vertices[i];
    m_pManualObject->position(v.position);
    m_pManualObject->normal(v.normal);
  }

  for(unsigned int i=0; i<m_indices.size(); i += 3)
    m_pManualObject->triangle(m_indices[i], m_indices[i+1], m_indices[i+2]);

  // reverse
  if (m_doubleSided)
  {
    for(unsigned int i=0; i<n; ++i)
    {
      Vertex& v = m_vertices[i];
      m_pManualObject->position(v.position);
      m_pManualObject->normal(-v.normal);
    }

    // swap vertex order
    for(unsigned int i=0; i<m_indices.size(); i += 3)
      m_pManualObject->triangle(n + m_indices[i], n + m_indices[i+2], n + m_indices[i+1]);
  }

  m_pManualObject->end();
}

void Geometry::setVertex(int i, const Ogre::Vector3& position, const Ogre::Vector3& normal)
{
  Vertex& v = m_vertices[i];
  v.position = position;
  v.normal = normal;
}

const Ogre::Vector3 Geometry::getVertexPosition(Ogre::uint32 index) const
{
  return m_vertices[index].position;
}

const std::vector<Vector3> Geometry::getVertexPositions() const
{
  std::vector<Vector3> positions;
  foreach(Vertex v, m_vertices)
    positions.push_back(v.position);
  return positions;
}