#include "../../StdAfx.h"
#include <OgrePanelOverlayElement.h>
#include "../../World/World.h"
#include "../../World/Things/FirstPerson.h"
#include "HUDOverviewMap.h"

HUDOverviewMap::HUDOverviewMap(const Config& config, 
															 World* pWorld, 
															 Ogre::PanelOverlayElement* pPanel)
: SingleOverlayElementHUDElement(pWorld, pPanel)
{
	m_viewSize = config.GetSetting<float>("HUDOverviewMap.ViewSize", 50000);
	m_firstPersonMarkerSize = config.GetSetting<float>("HUDOverviewMap.FirstPersonMarkerSize", 1.0);

	m_pTexture = Ogre::TextureManager::getSingleton().createManual("hud_overview_map_texture", 
		                                                             Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
																																 Ogre::TEX_TYPE_2D, 
																													       256, 256, 
																													       0, 
																													       Ogre::PF_R8G8B8, 
																													       Ogre::TU_RENDERTARGET );

	m_pRenderTarget = m_pTexture->getBuffer()->getRenderTarget();
	m_pRenderTarget->addListener(this);

  m_pCamera = pWorld->SceneManager->createCamera("hud_overview_map_camera");
  m_pCamera->setFarClipDistance(0);
  m_pCamera->setAspectRatio(1.0);
	m_pCamera->setFixedYawAxis(false);
	m_pCamera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
	m_pCamera->setOrthoWindowWidth(m_viewSize);
	//m_pCamera->setFOVy(Radian(Math::HALF_PI));
	m_pCamera->setNearClipDistance(10.0);

  m_pViewport = m_pRenderTarget->addViewport(m_pCamera);
  m_pViewport->setClearEveryFrame( true );
  m_pViewport->setBackgroundColour( ColourValue::Black );
	m_pViewport->setOverlaysEnabled(false);

	m_pMaterial = Ogre::MaterialManager::getSingleton().create("hud_overview_map_material",
                                                             Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  m_pMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("hud_overview_map_texture");

	m_pOverlayElement->setMaterialName("hud_overview_map_material");

	//// create marker for first person position
	//m_pFirstPersonMarker = pWorld->SceneManager->createEntity("hud_first_person_marker", "HUDFirstPersonMarker.mesh");
	//pWorld->SceneNode->createChildSceneNode()->attachObject(m_pFirstPersonMarker);
	//float firstPersonMarkerScale = m_firstPersonMarkerSize * m_viewSize / 50.0;
	//m_pFirstPersonMarker->getParentSceneNode()->setScale(pWorld->GetIsotropicScale(firstPersonMarkerScale));
}

HUDOverviewMap::~HUDOverviewMap()
{
	m_pRenderTarget->removeListener(this);
}

void HUDOverviewMap::Update()
{
	Vector3 position = m_pWorld->FirstPerson->WorldPosition;
	Quaternion orientation = m_pWorld->FirstPerson->WorldOrientation;

	// place camera directly above position
	Vector3 v = position + World::UP * 10000.0;
	m_pCamera->setPosition(v);

	// point downwards with local up direction pointing forwards
	// so, look in same direction as viewer...
	m_pCamera->setOrientation(orientation);
	// ...then look down, taking the shortest arc
	m_pCamera->setDirection(-World::UP);

	//// place first person marker
	//m_pFirstPersonMarker->getParentSceneNode()->setPosition(position);
	//Quaternion q;
	//q.FromAngleAxis(-Radian(Math::HALF_PI), Vector3::UNIT_X);
	//m_pFirstPersonMarker->getParentSceneNode()->setOrientation(orientation * q);
}

void 	HUDOverviewMap::preRenderTargetUpdate (const Ogre::RenderTargetEvent &evt)
{
	//m_pFirstPersonMarker->getParentSceneNode()->setVisible(true);
}

void 	HUDOverviewMap::postRenderTargetUpdate (const Ogre::RenderTargetEvent &evt)
{
	//m_pFirstPersonMarker->getParentSceneNode()->setVisible(false);
}