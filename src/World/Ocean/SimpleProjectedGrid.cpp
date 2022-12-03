#include "../../StdAfx.h"

#include "../World.h"
#include "SimpleProjectedGrid.h"

using namespace Ogre;

SimpleProjectedGrid::SimpleProjectedGrid(Camera* pCamera, int widthVerts, int heightVerts, ManualObject* pManualObject, const String& materialName)
	: m_plane(World::UP, 0),
    m_pCamera(pCamera),
    Grid(
      widthVerts, heightVerts,
      pManualObject,
      materialName
    )
{
  m_pProjectorCam = new Ogre::Camera("Projected Grid Cam", m_pCamera->getSceneManager());
  m_pProjectorCam->synchroniseBaseSettingsWith(m_pCamera);
}

SimpleProjectedGrid::~SimpleProjectedGrid()
{
}

void SimpleProjectedGrid::updateGeometry()
{
  updateProjector();

  m_pManualObject->beginUpdate(0);

  // vertices
  makeVerticesByInterpolationHomogeneous();

  // indices
  triangulate();

  m_pManualObject->end();
}

void SimpleProjectedGrid::updateProjector()
{
  m_pProjectorCam->synchroniseBaseSettingsWith(m_pCamera);

  m_unProjectionMatrix = m_pProjectorCam->getProjectionMatrix() * m_pProjectorCam->getViewMatrix(); 
  m_projectionMatrix = m_unProjectionMatrix.inverse();
}


void SimpleProjectedGrid::makeVerticesByProjectingAllPoints()
{
  for (float y=-1.0; y<=1.0; y+=(2.0f / (m_heightVerts - 1)))
  {
    for (float x=-1.0; x<=1.0; x+=(2.0f / (m_widthVerts - 1)))
    {
      m_pManualObject->position(project(Vector2(x, y)));
    }
  }
}

void SimpleProjectedGrid::makeVerticesByInterpolation()
{
  Vector3 topLeft     = project(Vector2(-1, +1));  
  Vector3 topRight    = project(Vector2(+1, +1));  
  Vector3 bottomLeft  = project(Vector2(-1, -1));  
  Vector3 bottomRight = project(Vector2(+1, -1));  

  for(int j=0; j<m_heightVerts; ++j)
  {
    float fUp = ((float)j / (m_heightVerts - 1));
    Vector3 left = bottomLeft + fUp * (topLeft - bottomLeft);
    Vector3 right = bottomRight + fUp * (topRight - bottomRight);
    Vector3 deltaLeftToRight = right - left;
    for(int i=0; i<m_widthVerts; ++i)
    {
      float fAcross = ((float) i / (m_widthVerts - 1));
      Vector3 p = left +  fAcross * deltaLeftToRight;
      m_pManualObject->position(p);
    }
  }
}

void SimpleProjectedGrid::makeVerticesByInterpolationHomogeneous()
{
  Vector4 topLeft     = projectHomogeneous(Vector2(-1, +1));  
  Vector4 topRight    = projectHomogeneous(Vector2(+1, +1));  
  Vector4 bottomLeft  = projectHomogeneous(Vector2(-1, -1));  
  Vector4 bottomRight = projectHomogeneous(Vector2(+1, -1));  

  for(int j=0; j<m_heightVerts; ++j)
  {
    float fUp = ((float)j / (m_heightVerts - 1));
    Vector4 left = bottomLeft + fUp * (topLeft - bottomLeft);
    Vector4 right = bottomRight + fUp * (topRight - bottomRight);
    Vector4 deltaLeftToRight = right - left;
    for(int i=0; i<m_widthVerts; ++i)
    {
      float fAcross = ((float) i / (m_widthVerts - 1));
      Vector4 p = left +  fAcross * deltaLeftToRight;

      m_pManualObject->position(p.x / p.w, p.y / p.w, p.z /p.w);
    }
  }
}

Vector3 SimpleProjectedGrid::project(const Ogre::Vector2& point)
{
  Vector3 from = m_projectionMatrix * Vector3(point.x, point.y, -1);
  Vector3 to   = m_projectionMatrix * Vector3(point.x, point.y, +1);

  Ray ray(from, from - to);

  std::pair<bool, Real> i = ray.intersects(m_plane);

  return ray.getPoint(i.second);
}

Vector4 SimpleProjectedGrid::projectHomogeneous(const Ogre::Vector2& point)
{
  // assumes plane normal is +z

  Vector4 from = m_projectionMatrix * Vector4(point.x, point.y, -1, 1);
  Vector4 to   = m_projectionMatrix * Vector4(point.x, point.y, +1, 1);

  Vector4 delta = to - from;

  Real t = (from.w * m_plane.d - from.z) / (delta.z - delta.w * m_plane.d);

  return from + t * delta;
}
