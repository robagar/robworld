#pragma once
#include "HUDElement.h"
#include "HUDThingLabel.h"

namespace Ogre
{
	class Overlay;
	class TextAreaOverlayElement;
}

class HUDOverviewMap;
class World;
class FirstPerson;

class HeadUpDisplay
{
	const World* m_pWorld;
	const FirstPerson* m_pFirstPerson;
	const Ogre::Camera* m_pCamera;

	bool m_visible;
	Ogre::Overlay* m_pOverlay;
	Ogre::TextAreaOverlayElement* m_pViewFocusInfo;
	Ogre::TextAreaOverlayElement* m_pDebugInfo;
	Ogre::TextAreaOverlayElement* m_pFirstPersonInfo;

	Ogre::RaySceneQuery* m_pPickingQuery;

	std::map<Thing*, HUDThingLabel*> m_autoLabelledThings;
	std::map<Thing*, HUDThingLabel*> m_labelledThings;
	std::list<HUDThingLabel*> m_labels;

	unsigned int m_maxLabelledThings; 

	std::vector<HUDElement*> m_elements;

public:
	HeadUpDisplay(const Config& config, World* pWorld);
	~HeadUpDisplay();

	const Ogre::Camera* GetCamera(){ return m_pCamera; }
	__declspec(property(get=GetCamera)) const Ogre::Camera* Camera;

	void AddElement(HUDElement* pElement);

	void Update();

	bool GetVisible() { return m_visible; }
	void SetVisible(bool visible);
	__declspec(property(get=GetVisible, put=SetVisible)) bool Visible;

	void SetDebugInfoText(String text);

	void AddThingLabel(Thing* pThing, String text, bool autoLabel = false);
	void RemoveThingLabel(Thing* pThing);

	bool ProjectToHUDSpace(const Vector3& vWorldSpace, Vector2& vHUDSpace);

	void GetThingsAt(Vector2& vHUDSpace, std::set<Thing*>& things, unsigned short maxThings = 0);
	void UpdateViewFocusInfo();

	void UpdateAutoLabelledThings();

};
