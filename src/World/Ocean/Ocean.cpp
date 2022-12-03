#include "../../StdAfx.h"
#include "../World.h"
#include "Ocean.h"

#include "SimpleProjectedGrid.h"
#include "Perturbation2DSineX.h"
#include "Ripples.h"

#include "OceanSurfaceReflectionCubeMap.h"

using namespace Ogre;

Ocean::Ocean(Camera* pCamera, const Config& config)
  : m_surface(config.GetSetting<Real>("SeaLevel")),
    m_grid(&m_surface, 
           pCamera, 
           //256, 128, 
           64, 128,
           pCamera->getSceneManager()->createManualObject("Ocean Projected Grid"), 
           "OceanSurfaceMaterial",
           100,
           1000)
{
  SceneManager* pSceneManager = pCamera->getSceneManager();

  m_pSceneNode = pSceneManager->createSceneNode("Ocean Node");
  pSceneManager->getRootSceneNode()->addChild(m_pSceneNode);

  m_pSceneNode->attachObject(m_grid.ManualObjectPtr);

  //m_grid.addPerturbation(new Perturbation2DSineX(0.25, 20.0, 5.0));
  m_grid.addPerturbation(new Ripples());

  m_pCubeMap = new OceanSurfaceReflectionCubeMap(this, pCamera->getSceneManager(), config);
}

Ocean::~Ocean()
{
  delete m_pCubeMap;
}


void Ocean::Update(Real time, Real deltaTime)
{
  m_time = time;
  m_deltaTime = deltaTime;
  m_grid.update(time, deltaTime);
}

String Ocean::getDebugText()
{
  return m_grid.getDebugText();
}