#pragma once

#include <OgreTextAreaOverlayElement.h>
#include "SingleOverlayElementHUDElement.h"
#include "../../World/Things/FirstPerson.h"


class FirstPersonLocationHUDElement : public SingleOverlayElementHUDElement<Ogre::TextAreaOverlayElement>
{
public:
	FirstPersonLocationHUDElement(World* pWorld, 
																Ogre::TextAreaOverlayElement* pTextAreaOverlayElement);
	void Update();
};