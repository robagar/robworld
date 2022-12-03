#include "../../StdAfx.h"
#include "TerrainHeightHUDElement.h"

using namespace NonUniformTerrain;

TerrainHeightHUDElement::TerrainHeightHUDElement(World* pWorld)
: HUDElement(pWorld),
	m_pHeightShader(m_pWorld->Terrain->HeightShader)
{
	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
	m_pPanel = dynamic_cast<Ogre::PanelOverlayElement*>(overlayManager.getOverlayElement("terrain_height"));
	m_pHeightShaderPanel = dynamic_cast<Ogre::PanelOverlayElement*>(overlayManager.getOverlayElement("terrain_height_shader"));

	// TODO: move into Update with check for changes shader
	const Ogre::MaterialPtr& pMaterial = m_pHeightShaderPanel->getMaterial();
	Ogre::TextureUnitState* pTextureUnit = pMaterial->getTechnique(0)->getPass(0)->getTextureUnitState("height_colours");
	pTextureUnit->setTextureName(m_pHeightShader->TextureName);

	// marked heights
	Ogre::TextAreaOverlayElement* pLabelPrototype = dynamic_cast<Ogre::TextAreaOverlayElement*>(overlayManager.getOverlayElement("terrain_height_label_prototype"));
	m_heightShaderPanelHeight = m_pHeightShaderPanel->getHeight();
	m_heightBaseY = m_pHeightShaderPanel->getTop() - (pLabelPrototype->getCharHeight() / 2);
	float labelX = m_pHeightShaderPanel->getLeft() + m_pHeightShaderPanel->getWidth() + 10;
	foreach(TerrainHeightShaderMarkedHeight m, m_pHeightShader->Heights)
	{
		if (!m.showLabel)
			continue;

		std::ostringstream o;
		o << "terrain_height_label " << m.height << "m";
		Ogre::TextAreaOverlayElement* pHeightLabel = dynamic_cast<Ogre::TextAreaOverlayElement*>(pLabelPrototype->clone(o.str()));

		o.str("");
		o << (m.height == 0 ? 0 : -m.height) << "m";
		pHeightLabel->setCaption(o.str());
		pHeightLabel->setPosition(labelX, GetYForHeight(m.height));
		pHeightLabel->show();
		m_pPanel->addChild(pHeightLabel);
	}

	// arrow
	m_pArrow = dynamic_cast<Ogre::PanelOverlayElement*>(overlayManager.getOverlayElement("terrain_height_arrow"));
	m_pArrowLabel = dynamic_cast<Ogre::TextAreaOverlayElement*>(overlayManager.getOverlayElement("terrain_height_arrow_label"));
}

float TerrainHeightHUDElement::GetYForHeight(float height)
{
	float v = m_pHeightShader->GetVForHeight(height);
	v = clamp(v, 0.0f, 1.0f);
	return m_heightBaseY + m_heightShaderPanelHeight * v;
}

void TerrainHeightHUDElement::Update()
{
	const Vector3& v = m_pWorld->FirstPerson->WorldPosition;
	float height = m_pWorld->Terrain->GetHeight(v.x, v.y);

	float y = GetYForHeight(height);
	m_pArrow->setTop(y);
	
	std::ostringstream o;
	o << setprecision(0) << (int)(height == 0 ? 0 : -height) << "m";
	m_pArrowLabel->setCaption(o.str());
	m_pArrowLabel->setTop(y - 15);
}