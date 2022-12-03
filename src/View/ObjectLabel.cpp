#include "../StdAfx.h"
#include "ObjectLabel.h"

Ogre::Overlay* ObjectLabel::m_pOverlay = NULL;

unsigned int ObjectLabel::s_nextLabelID = 0;

ObjectLabel::ObjectLabel(const Ogre::MovableObject* p, const Ogre::Camera* c) 
{
	// create an overlay for all labels
	if (m_pOverlay == NULL)
		m_pOverlay = Ogre::OverlayManager::getSingleton().create("labels_overlay");

	m_p = p;
	m_c = c;
	m_enabled = false;
	m_text = "";

	std::ostringstream s;
	s << "label_" << s_nextLabelID++; 
	m_name = s.str();

	s << "_container";
	String containerName = s.str();
	
	m_pContainer = static_cast<Ogre::PanelOverlayElement*>(Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", containerName));
	m_pOverlay->add2D(m_pContainer);
	m_pContainer->setMaterialName("RedMaterial");
	m_pContainer->setTransparent(false);
	m_pContainer->setColour(ColourValue::Red);

	m_pText = Ogre::OverlayManager::getSingleton().createOverlayElement("TextArea", m_name);
	m_pText->setDimensions(1.0, 1.0);
	m_pText->setMetricsMode(Ogre::GMM_PIXELS);
	m_pText->setPosition(0, 0);

	m_pText->setParameter("font_name", "Eurostile-28pt");
	m_pText->setParameter("char_height", "20");
	m_pText->setParameter("horz_align", "center");
	m_pText->setColour(Ogre::ColourValue(0.0, 0.0, 0.6));

	m_pContainer->addChild(m_pText);
	m_pOverlay->show();
}

ObjectLabel::~ObjectLabel()
{
	// overlay cleanup -- Ogre would clean this up at app exit but if your app 
	// tends to create and delete these objects often it's a good idea to do it here.

	m_pOverlay->hide();
	Ogre::OverlayManager *overlayManager = Ogre::OverlayManager::getSingletonPtr();
	m_pContainer->removeChild(m_name);
	m_pOverlay->remove2D(m_pContainer);
	overlayManager->destroyOverlayElement(m_pText);
	overlayManager->destroyOverlayElement(m_pContainer);
	overlayManager->destroy(m_pOverlay);
}


bool	ProjectPos	(const Ogre::Camera* cam,const Ogre::Vector3& pos,Ogre::Real& x,Ogre::Real& y) {
	Vector3 eyeSpacePos = cam->getViewMatrix(true) * pos;
	// z < 0 means in front of cam
	if (eyeSpacePos.z < 0) {
		// calculate projected pos
		Vector3 screenSpacePos = cam->getProjectionMatrix() * eyeSpacePos;
		x = screenSpacePos.x;
		y = screenSpacePos.y;
		return true;
	} else {
		x = (-eyeSpacePos.x > 0) ? -1 : 1;
		y = (-eyeSpacePos.y > 0) ? -1 : 1;
		return false;
	}
}

void ObjectLabel::update()  
{
	if (!m_enabled)
		return;

	Vector3 v = m_p->getWorldBoundingBox(true).getCenter();

	float x, y;
	if (ProjectPos(m_c, v, x, y))
		m_pContainer->setPosition((x+1)/2, (y+1)/2);
	m_pContainer->setDimensions(0.1, 0.1);

	std::ostringstream o;
	o << x << ", " << y;
	m_pText->setCaption(o.str());

	//// get the projection of the object's AABB into screen space
	//const Ogre::AxisAlignedBox& bbox = m_p->getWorldBoundingBox(true);
	//Ogre::Matrix4 mat = m_c->getViewMatrix();

	//const Ogre::Vector3* corners = bbox.getAllCorners();

	//float min_x = 1.0f, max_x = 0.0f, min_y = 1.0f, max_y = 0.0f;

	//// expand the screen-space bounding-box so that it completely encloses 
	//// the object's AABB
	//for (int i=0; i<8; i++) {
	//	Ogre::Vector3 corner = corners[i];

	//	// multiply the AABB corner vertex by the view matrix to 
	//	// get a camera-space vertex
	//	corner = mat * corner;

	//	// make 2D relative/normalized coords from the view-space vertex
	//	// by dividing out the Z (depth) factor -- this is an approximation
	//	float x = corner.x / corner.z + 0.5;
	//	float y = corner.y / corner.z + 0.5;

	//	if (x < min_x) 
	//		min_x = x;

	//	if (x > max_x) 
	//		max_x = x;

	//	if (y < min_y) 
	//		min_y = y;

	//	if (y > max_y) 
	//		max_y = y;
	//}

	//// we now have relative screen-space coords for the object's bounding box; here
	//// we need to center the text above the BB on the top edge. The line that defines
	//// this top edge is (min_x, min_y) to (max_x, min_y)

	////m_pContainer->setPosition(1-min_x, min_y);
	//m_pContainer->setPosition(0.5, 0.5);
	//m_pContainer->setDimensions(max_x - min_x, 0.1);
}