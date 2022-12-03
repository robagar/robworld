#include "../../StdAfx.h"
#include "HUDThingLabel.h"
#include "HUD.h"
#include "../../World/Things/Thing.h"

unsigned int HUDThingLabel::s_nextLabelID = 0;

HUDThingLabel::HUDThingLabel(HeadUpDisplay* pHUD, Thing* pThing, std::string text)
: m_pHUD(pHUD), m_pThing(pThing), m_text(text)
{
	std::ostringstream s;
	s << "label_" << s_nextLabelID++; 
	m_name = s.str();

	s << "_container";
	String containerName = s.str();
	
	m_pContainer = static_cast<Ogre::PanelOverlayElement*>(Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", containerName));
	//m_pContainer->setMaterialName("RedMaterial");
	//m_pContainer->setTransparent(false);
	//m_pContainer->setColour(ColourValue::Blue);

	m_pText = Ogre::OverlayManager::getSingleton().createOverlayElement("TextArea", m_name);
	m_pText->setDimensions(1.0, 1.0);
	m_pText->setMetricsMode(Ogre::GMM_PIXELS);
	m_pText->setPosition(0, 0);
	m_pText->setCaption(m_text);

	m_pText->setParameter("font_name", "Eurostile-28pt");
	m_pText->setParameter("char_height", "24");
	m_pText->setParameter("horz_align", "left");
	m_pText->setColour(Ogre::ColourValue(0.0f, 0.0f, 0.6f));

	m_pContainer->addChild(m_pText);
}

void HUDThingLabel::Update()
{
	Vector2 vScreenSpace;
	if (m_pHUD->ProjectToHUDSpace(m_pThing->WorldPosition, vScreenSpace))
	{
		m_pContainer->setPosition(vScreenSpace.x, vScreenSpace.y); 
		m_pContainer->setDimensions(1.0f, 0.1f);
		m_pContainer->show();
	}
	else
	{
		m_pContainer->hide();
	}
}