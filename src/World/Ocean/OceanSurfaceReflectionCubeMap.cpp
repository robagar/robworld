#include "../../StdAfx.h"
#include "../World.h"
#include "Ocean.h"
#include "OceanSurfaceReflectionCubeMap.h"

OceanSurfaceReflectionCubeMap::OceanSurfaceReflectionCubeMap(Ocean* pOcean, Ogre::SceneManager* pSceneManager, const Config& config)
  : m_pOcean(pOcean)
{
  Real t = config.GetSetting<Real>("OceanSurfaceReflectionCubeMapUpdateDistanceThreshold");
  m_updateDistanceThresholdSquared = t * t;

  initTexture(config);
  initCamera(pSceneManager->createCamera("OceanReflectionCubeMapCam"));
  initRenderTargets();
  initOceanSurfaceMaterial();

  Root::getSingleton().addFrameListener(this);

  refresh();
}

void OceanSurfaceReflectionCubeMap::initTexture(const Config& config)
{
  m_pTexture = Ogre::TextureManager::getSingleton().createManual(
    "OceanSurfaceReflectionCubeMap",
    Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
    Ogre::TEX_TYPE_CUBE_MAP, 
    config.GetSetting<uint>("OceanSurfaceReflectionCubeMapWidth"), 
    config.GetSetting<uint>("OceanSurfaceReflectionCubeMapHeight"), 
    1, 0, Ogre::PF_A8R8G8B8, Ogre::TU_RENDERTARGET
  );
}

void OceanSurfaceReflectionCubeMap::initCamera(Ogre::Camera* pCamera)
{
  m_pCamera = pCamera;

  m_pCamera->setNearClipDistance(0.1f);
  m_pCamera->setFarClipDistance(500.f);
  m_pCamera->setAspectRatio(1);
  m_pCamera->setFOVy(Ogre::Radian(Ogre::Math::PI / 2));

  // orientations for each rendertarget (determined by trial and error)
  m_cameraOrientations[0].FromAxes(World::SOUTH, World::UP, World::WEST);
  m_cameraOrientations[1].FromAxes(World::NORTH, World::UP, World::EAST);
  m_cameraOrientations[2].FromAxes(World::EAST, World::SOUTH, World::DOWN);
  m_cameraOrientations[3] = Quaternion::IDENTITY;
  m_cameraOrientations[4].FromAxes(World::EAST, World::UP, World::SOUTH);
  m_cameraOrientations[5].FromAxes(World::WEST, World::UP, World::NORTH);
}

void OceanSurfaceReflectionCubeMap::initRenderTargets()
{
  for (int i=0; i<6; i++)
  { 
    Ogre::RenderTarget* pRenderTarget = m_renderTargets[i] = m_pTexture->getBuffer(i)->getRenderTarget(); 

    pRenderTarget->setAutoUpdated(false);

    Ogre::Viewport* pViewport = pRenderTarget->addViewport(m_pCamera); 
    pViewport->setOverlaysEnabled(false);
    pViewport->setBackgroundColour( Ogre::ColourValue::Black );

    pRenderTarget->addListener(this);
  }
}

void OceanSurfaceReflectionCubeMap::initOceanSurfaceMaterial()
{
  // set cubemap in material
  MaterialPtr pMaterial = Ogre::MaterialManager::getSingleton().getByName("OceanSurfaceMaterial");
  Ogre::Pass* pPass = pMaterial->getTechnique(0)->getPass(0);
  Ogre::TextureUnitState* pTextureUnitState = pPass->getTextureUnitState("OceanSurfaceReflectionCubeMap");
  pTextureUnitState->setTextureName("OceanSurfaceReflectionCubeMap");
}

OceanSurfaceReflectionCubeMap::~OceanSurfaceReflectionCubeMap()
{
  for (int i=0; i<6; i++)
    m_renderTargets[i]->removeListener(this);

  TextureManager::getSingleton().remove("OceanSurfaceReflectionCubeMap");
}

void OceanSurfaceReflectionCubeMap::preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt)
{
  m_pOcean->setVisible(false);

  m_pCamera->setOrientation(renderTargetCameraOrientation(evt.source));
}

void OceanSurfaceReflectionCubeMap::postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt)
{
  m_pOcean->setVisible(true);
}

bool OceanSurfaceReflectionCubeMap::frameStarted(const FrameEvent& evt)
{
  static int renderTargetToUpdate = 6;
  static bool updateInProgress = false;

  if (viewpointHasCrossedSurface())
    m_updateAllRenderTargets = true;
  else if (!updateInProgress && viewpointHasMovedFarEnough())
  {
    m_pCamera->setPosition(World::instance()->FirstPerson->WorldPosition); 
    renderTargetToUpdate = 0;
    updateInProgress = true;
  }

  if (m_updateAllRenderTargets)
  {
    debug << "OceanSurfaceReflectionCubeMap: updating all render targets " << std::endl;

    for (int i=0; i<6; i++)
      m_renderTargets[i]->update();

    m_updateAllRenderTargets = false;
    updateInProgress = false;
  }
  else if (renderTargetToUpdate < 6)
  {
    // update one target each frame until all six are done
    //debug << "OceanSurfaceReflectionCubeMap: updating render target " << renderTargetToUpdate << std::endl;
    m_renderTargets[renderTargetToUpdate++]->update();
  }
  else
    updateInProgress = false;

  return true;
}

bool OceanSurfaceReflectionCubeMap::viewpointHasMovedFarEnough()
{
  return distanceFromLastUpdateSquared() > m_updateDistanceThresholdSquared;
}

bool OceanSurfaceReflectionCubeMap::viewpointHasCrossedSurface()
{
  static bool viewpointIsUnderwater = false;
  bool previous = viewpointIsUnderwater;

  const Vector3& p = World::instance()->FirstPerson->WorldPosition;
  
  viewpointIsUnderwater = m_pOcean->isUnderwaterAt(p);
  return viewpointIsUnderwater != previous;
}


bool OceanSurfaceReflectionCubeMap::frameEnded(const FrameEvent& evt)
{
  //m_renderTargets[0]->writeContentsToFile("OceanSurfaceReflectionCubeMap_0.png");
  //m_renderTargets[1]->writeContentsToFile("OceanSurfaceReflectionCubeMap_1.png");
  //m_renderTargets[2]->writeContentsToFile("OceanSurfaceReflectionCubeMap_2.png");
  //m_renderTargets[3]->writeContentsToFile("OceanSurfaceReflectionCubeMap_3.png");
  //m_renderTargets[4]->writeContentsToFile("OceanSurfaceReflectionCubeMap_4.png");
  //m_renderTargets[5]->writeContentsToFile("OceanSurfaceReflectionCubeMap_5.png");

  //Root::getSingleton().removeFrameListener(this);

  return true;
}

Ogre::Real OceanSurfaceReflectionCubeMap::distanceFromLastUpdateSquared()
{
  const Vector3& p = World::instance()->FirstPerson->WorldPosition;
  return p.squaredDistance(m_pCamera->getPosition());
}

const Ogre::Quaternion& OceanSurfaceReflectionCubeMap::renderTargetCameraOrientation(const RenderTarget* pRenderTarget)
{
  return m_cameraOrientations[renderTargetIndex(pRenderTarget)];
}

int OceanSurfaceReflectionCubeMap::renderTargetIndex(const RenderTarget* pRenderTarget)
{
  for(int i=0; i<6; ++i)
    if (m_renderTargets[i] == pRenderTarget)
      return i;

  return -1;
}