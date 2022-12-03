#include "../../StdAfx.h"
#include "../../World/World.h"
#include "../../Geo/DegMinSec.h"
#include "FirstPersonLocationHUDElement.h"

FirstPersonLocationHUDElement::FirstPersonLocationHUDElement(World* pWorld, Ogre::TextAreaOverlayElement* pTextAreaOverlayElement)
: SingleOverlayElementHUDElement(pWorld, pTextAreaOverlayElement)
{
}

void FirstPersonLocationHUDElement::Update()
{
	std::wostringstream o;

	// lat/long position
	const Geo::GeoLocation* pl = &m_pWorld->FirstPerson->Location;
	o << latitude_degminsec(pl->latitude) << endl;
	o << longitude_degminsec(pl->longitude) << endl;

	// heading
	o << std::setprecision(0);
	o << std::setw(3);
	o << std::setfill(L'0');
	o << "Heading: " << m_pWorld->FirstPerson->Heading << endl;

	// altitude / depth
	float h = m_pWorld->FirstPerson->WorldPosition.y;
	o << std::setprecision(1);
	if (h > 0)
		o << "Altitude: " << h << "m" << endl;
	else
		o << "Depth: " << h << "m" << endl;

	m_pOverlayElement->setCaption(o.str());
}
