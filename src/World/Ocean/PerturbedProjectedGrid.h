#pragma once

#include "../../3D/Grid.h"
#include "PerturbedHorizontal.h"

class PerturbedProjectedGrid : public Grid
{
	Ogre::Camera* m_pRealRenderCam;
  Ogre::Camera* m_pRenderCam;
	Ogre::Camera* m_pProjectorCam;

  PerturbedHorizontal* m_pSurface;

  Ogre::Real m_minProjectorHeight;
  Ogre::Real m_maxLookAtPointDistance;
  Ogre::Vector3 m_projectorLookAtPoint; 

  Ogre::Matrix4 m_projectionMatrix;
  Ogre::Matrix4 m_unProjectionMatrix;

  Ogre::Matrix4 m_rangeConversionMatrix;

  Ogre::Vector2 m_rangeMin, m_rangeMax;

  std::vector<Ogre::Vector3> m_vertexPositions;

public:
  PerturbedProjectedGrid(PerturbedHorizontal* pSurface, 
                         Ogre::Camera* pCamera, 
                         int widthVerts, int heightVerts, 
                         Ogre::ManualObject* pManualObject, 
                         const Ogre::String& materialName,
                         Ogre::Real minProjectorHeight,
                         Ogre::Real maxLookAtPointDistance);
  ~PerturbedProjectedGrid();

  void addPerturbation(Perturbation2D* p) { m_pSurface->addPerturbation(p); }

  virtual void update(Ogre::Real time, Ogre::Real deltaTime);

  Ogre::Vector3& vertexPosition(Ogre::uint32 i, Ogre::uint32 j) { return m_vertexPositions.at(i + m_widthVerts * j); }

  String getDebugText();

private:
  void updateProjector();
  bool determineVisibility();

  void positionVerticesOnBasePlane();
  void perturbVertices(Ogre::Real time, Ogre::Real deltaTime);

  void positionVerticesByInterpolationHomogeneous();
  void positionVerticesByUVInterpolationHomogeneous();

  Ogre::Vector3 calculateNormal(Ogre::uint32 i, Ogre::uint32 j);

  void makeVertices();

  Ogre::Vector4 projectHomogeneous(const Ogre::Vector2&);
};