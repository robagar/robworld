#pragma once

#include "PerturbedProjectedGrid.h"

class OceanSurfaceReflectionCubeMap;

class Ocean
{
  Ogre::SceneNode* m_pSceneNode;

  PerturbedHorizontal m_surface;
  PerturbedProjectedGrid m_grid;

  Ogre::Real m_time, m_deltaTime;

  OceanSurfaceReflectionCubeMap* m_pCubeMap;

public:
	Ocean(Ogre::Camera* pCamera, const Config& config);
	~Ocean();


	bool isVisible() { return true; /* m_pOceanSurface->isVisible();*/ }
	void setVisible(bool visible) { m_pSceneNode->setVisible(visible); }

	void Update(Ogre::Real time, Ogre::Real deltaTime);

  Real levelAt(Ogre::Real x, Ogre::Real y) { return m_surface.levelAt(x, y, m_time, m_deltaTime); }
  Real positionDepth(const Vector3& position) { return levelAt(position.x, position.y) - position.z; }
  bool isUnderwaterAt(const Vector3& p) { return levelAt(p.x, p.y) > p.z; }

  String getDebugText();
};