#pragma once

class Ocean;

class OceanSurfaceReflectionCubeMap : public Ogre::RenderTargetListener, Ogre::FrameListener
{
  Ocean* m_pOcean;

  Ogre::Camera* m_pCamera;
  Ogre::TexturePtr m_pTexture;
  Ogre::RenderTarget* m_renderTargets[6];
  Ogre::Quaternion m_cameraOrientations[6];

  Ogre::Real m_updateDistanceThresholdSquared;
  bool m_updateAllRenderTargets;

public:
  OceanSurfaceReflectionCubeMap(Ocean* pOcean, Ogre::SceneManager* pSceneManager, const Config& config);
  virtual ~OceanSurfaceReflectionCubeMap();

  void refresh() { m_updateAllRenderTargets = true; }

  // RenderTargetListener methods
  virtual void preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt);
  virtual void postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt);

  // FrameListener methods
  virtual bool frameStarted (const FrameEvent &evt);
  virtual bool frameEnded (const FrameEvent &evt);

private:
  void initTexture(const Config& config);
  void initCamera(Ogre::Camera* pCamera);
  void initRenderTargets();
  void initOceanSurfaceMaterial();

  bool viewpointHasMovedFarEnough();
  bool viewpointHasCrossedSurface();

  Ogre::Real distanceFromLastUpdateSquared();
  const Ogre::Quaternion& renderTargetCameraOrientation(const RenderTarget*);
  int renderTargetIndex(const RenderTarget*);
};