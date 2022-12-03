#pragma once
#include "HUDElement.h"
#include "../../World/Terrain/Terrain.h"
#include "../../World/Terrain/TerrainHeightShader.h"

class TerrainHeightHUDElement : public HUDElement
{
	const NonUniformTerrain::TerrainHeightShader* m_pHeightShader; 

	Ogre::PanelOverlayElement* m_pPanel;
	Ogre::PanelOverlayElement* m_pHeightShaderPanel;
	Ogre::PanelOverlayElement* m_pArrow;
	Ogre::TextAreaOverlayElement* m_pArrowLabel;

	float m_heightShaderPanelHeight;
	float m_heightBaseY;

public:
	TerrainHeightHUDElement(World* pWorld);
	void Update();

private:
	float GetYForHeight(float height);
};

