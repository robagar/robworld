#pragma once

#include <OgrePanelOverlayElement.h>

class HeadUpDisplay;
class Thing;

class HUDThingLabel
{
	static unsigned int s_nextLabelID;

	HeadUpDisplay* m_pHUD;

	Thing* m_pThing;
	std::string m_text;

	std::string m_name;
	Ogre::OverlayElement* m_pText;
	Ogre::PanelOverlayElement* m_pContainer;

public:
	HUDThingLabel(HeadUpDisplay* pHUD, Thing* pThing, std::string text);

	Ogre::OverlayContainer* GetContainer() { return m_pContainer; }
	__declspec(property(get=GetContainer)) Ogre::OverlayContainer* Container;

	void Update();
};