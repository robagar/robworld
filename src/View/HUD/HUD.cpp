#include "../../StdAfx.h"
//#include "OgreFontUtils.h"
#include <OgreTextAreaOverlayElement.h>
#include "HUD.h"
#include "HUDOverviewMap.h"
#include <OgreFontManager.h>
#include "../../World/World.h"
#include "../../World/Things/FirstPerson.h"
//#include "UserData/UserDataItem.h"

#include "FirstPersonLocationHUDElement.h"
#include "TerrainHeightHUDElement.h"

//using namespace UserData;

#define QUERY_MASK_ALL 0xffffffff

HeadUpDisplay::HeadUpDisplay(const Config& config, World* pWorld)
:	m_pWorld(pWorld),
	m_pFirstPerson(pWorld->FirstPerson),
	m_pCamera(pWorld->FirstPerson->Camera),
	m_pPickingQuery(NULL),
	m_maxLabelledThings(3),
	m_visible(false)
{
	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
	m_pOverlay = overlayManager.getByName("hud");
	m_pViewFocusInfo = dynamic_cast<Ogre::TextAreaOverlayElement*>(overlayManager.getOverlayElement("view_focus_info"));
	m_pDebugInfo = dynamic_cast<Ogre::TextAreaOverlayElement*>(overlayManager.getOverlayElement("debug_info"));

	// initialize picking scene query with dummy ray
	Ray ray;
	m_pPickingQuery = pWorld->SceneManager->createRayQuery(ray, QUERY_MASK_ALL);

	//// initialize overview map
	//AddElement(new HUDOverviewMap(config, 
	//															pWorld, 
	//															dynamic_cast<Ogre::PanelOverlayElement*>(overlayManager.getOverlayElement("overview_map"))));

	// first person location
	AddElement(new FirstPersonLocationHUDElement(pWorld, 
																							 dynamic_cast<Ogre::TextAreaOverlayElement*>(overlayManager.getOverlayElement("first_person_location"))));

	// terrain height
	//AddElement(new TerrainHeightHUDElement(pWorld));
}

HeadUpDisplay::~HeadUpDisplay()
{
	foreach(HUDElement* pElement, m_elements)
		delete pElement;
}

void HeadUpDisplay::SetVisible(bool visible)
{
	if (visible)
	{
		m_pOverlay->show();
		m_visible = true;
	}
	else
	{
		m_pOverlay->hide();
		m_visible = false;
	}
}

void HeadUpDisplay::AddElement(HUDElement* pElement)
{
	m_elements.push_back(pElement);
}

void HeadUpDisplay::SetDebugInfoText(String text)
{
	m_pDebugInfo->setCaption(text);
}

void HeadUpDisplay::Update()
{
	UpdateViewFocusInfo();

	//UpdateFirstPersonInfo();
	//UpdateAutoLabelledThings();

	// set label positions
	foreach(HUDThingLabel* pLabel, m_labels)
		pLabel->Update();

	// update all elements
	foreach(HUDElement* pElement, m_elements)
		pElement->Update();
}

void HeadUpDisplay::AddThingLabel(Thing* pThing, String text, bool autoLabel)
{
	assert(pThing);
	assert(m_labelledThings.find(pThing) == m_labelledThings.end());

	debug << "HUD: adding label to " << pThing->Name << endl;

	HUDThingLabel* pLabel = new HUDThingLabel(this, pThing, text);
	m_labels.push_back(pLabel);
	m_pOverlay->add2D(pLabel->Container);
	m_labelledThings[pThing] = pLabel;
	if (autoLabel)
		m_autoLabelledThings[pThing] = pLabel;
}

void HeadUpDisplay::RemoveThingLabel(Thing* pThing)
{
	assert(pThing);
	assert(m_labelledThings.find(pThing) != m_labelledThings.end());

	debug << "HUD: removing label from " << pThing->Name << endl;

	HUDThingLabel* pLabel = m_labelledThings.find(pThing)->second;
	m_pOverlay->remove2D(pLabel->Container);
	m_labels.remove(pLabel);
	m_labelledThings.erase(pThing);
	m_autoLabelledThings.erase(pThing);
}

bool HeadUpDisplay::ProjectToHUDSpace(const Vector3& vWorldSpace, Vector2& vHUDSpace)
{
	Vector3 vEyeSpace = m_pCamera->getViewMatrix(true) * vWorldSpace;

	if (vEyeSpace.z < 0) 
	{
		//  in front of camera (but not necessarily visible)
		Vector3 vScreenSpace = m_pCamera->getProjectionMatrix() * vEyeSpace;

		// normalize, converting from range [-1, 1] to [0,1]
		vHUDSpace.x = (vScreenSpace.x + 1.0f) * 0.5f;
		vHUDSpace.y = (-vScreenSpace.y + 1.0f) * 0.5f;

		return true;
	} 
	else 
	{
		// behind camera
		return false;
	}
}

void HeadUpDisplay::GetThingsAt(Vector2& vHUDSpace, std::set<Thing*>& things, unsigned short maxThings)
{
	
	Ray ray = m_pCamera->getCameraToViewportRay(vHUDSpace.x, vHUDSpace.y);
	m_pPickingQuery->setRay(ray);

	m_pPickingQuery->setSortByDistance(true, maxThings);

	// Execute
	Ogre::RaySceneQueryResult results = m_pPickingQuery->execute();

	foreach(Ogre::RaySceneQueryResultEntry r, results)
	{
    debug << "picked " << r.movable->getName() << endl;
		//Ogre::Any object = r.movable->getUserAny();
		//Thing* pThing = static_cast<Thing*>(&object);
		//things.insert(pThing);
	}

	m_pPickingQuery->clearResults();
}

void HeadUpDisplay::UpdateViewFocusInfo()
{
}

//void HeadUpDisplay::UpdateFirstPersonInfo()
//{
//	wostringstream o;
//
//	Vector3 v = m_pFirstPerson->Position;
//	Geo::GeoLocation l;
//	m_pWorld->Projection.XYToGeoLocation(v, l);
//	o << latitude_degminsec(l.latitude) << endl;
//	o << longitude_degminsec(l.longitude) << endl;
//
//	//o << setiosflags(ios::fixed);
//
//	//o << setprecision(0);
//	//o << setw(3);
//	//o << setfill(L'0');
//	//o << "Heading: " << m_pFirstPerson->Heading << endl;
//
//	//o << setprecision(1);
//
//	////o << "Position: " << setprecision(6) << v.x << ", " << v.y << endl;
//	//o << "Altitude: " << v.z << "m" << endl;
//	//
//	//float terrainHeight = m_pWorld->GetTerrainHeight(v.x, v.y);
//	//if (terrainHeight < 0)
//	//o << "Depth: " << -terrainHeight << "m" << endl;
//	//else
//	//	o << "Terrain height: " << terrainHeight << "m" << endl;
//
//	////v = m_pCamera->getDirection();
//	////o << "Camera direction: " << v;
//	SetFirstPersonInfoText(o.str());
//}


void HeadUpDisplay::UpdateAutoLabelledThings()
{
	std::set<Thing*> things;
	GetThingsAt(Vector2(0.5f, 0.5f), things, m_maxLabelledThings);

	// remove old labels
	std::vector<Thing*> thingsToRemove;
	for(std::map<Thing*, HUDThingLabel*>::iterator it = m_autoLabelledThings.begin();
			it != m_autoLabelledThings.end();
			++it)
	{
		Thing* pThing = it->first;
		if (!contains(things, pThing))
			thingsToRemove.push_back(pThing);
	}

	foreach(Thing* pThing, thingsToRemove)
		RemoveThingLabel(pThing);

	// add new labels
	foreach(Thing* pThing, things)
	{
		if (pThing && !contains(m_autoLabelledThings, pThing))
		{
			AddThingLabel(pThing, pThing->Name, true);
		}
	}
}