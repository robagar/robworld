#pragma once

#include "../../3D/Grid.h"

class SimpleProjectedGrid : public Grid
{
protected:
  Ogre::Plane m_plane;

	Ogre::Camera* m_pCamera;
	Ogre::Camera* m_pProjectorCam;

  Ogre::Matrix4 m_projectionMatrix;
  Ogre::Matrix4 m_unProjectionMatrix;


public:
	SimpleProjectedGrid(Ogre::Camera* pCamera, int widthVerts, int heightVerts, Ogre::ManualObject* pManualObject, const Ogre::String& materialName);
	~SimpleProjectedGrid();


	virtual void updateGeometry();
  virtual void updateProjector();

  Ogre::Vector3 project(const Ogre::Vector2&);
  Ogre::Vector4 projectHomogeneous(const Ogre::Vector2&);

  void makeVerticesByInterpolation();
  void makeVerticesByInterpolationHomogeneous();
  void makeVerticesByProjectingAllPoints();

};

