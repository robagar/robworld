#pragma once
#include "SingleOverlayElementHUDElement.h"

class HUDOverviewMap : public SingleOverlayElementHUDElement<Ogre::PanelOverlayElement>, Ogre::RenderTargetListener
{
	Ogre::Camera* m_pCamera;
	Ogre::Viewport* m_pViewport;
	Ogre::MaterialPtr m_pMaterial;
	Ogre::TexturePtr m_pTexture;
	Ogre::RenderTarget* m_pRenderTarget;

	float m_viewSize;
	float m_firstPersonMarkerSize;

	Ogre::Entity* m_pFirstPersonMarker;

public:
	HUDOverviewMap(const Config& config, World* pWorld, Ogre::PanelOverlayElement* pPanel);
	~HUDOverviewMap();

	// RenderTargetListener
	void 	preRenderTargetUpdate (const Ogre::RenderTargetEvent& evt);
	void 	postRenderTargetUpdate (const Ogre::RenderTargetEvent& evt);

	void Update();
};