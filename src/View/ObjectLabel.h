#pragma once

#include <OgrePanelOverlayElement.h>

class ObjectLabel 
{

public:
	ObjectLabel(const Ogre::MovableObject* p, const Ogre::Camera* c);

	virtual ~ObjectLabel();

	void enable(bool enable) {
		m_enabled = enable;
		if (enable)
			m_pOverlay->show();
		else
			m_pOverlay->hide();
	}

	void setText(const Ogre::String& text) {
		m_text = text;
		m_pText->setCaption(m_text);
	}

	void update();

private:
	static unsigned int s_nextLabelID;

	static Ogre::Overlay* m_pOverlay;

	std::string m_name;

	const Ogre::MovableObject* m_p;
	const Ogre::Camera* m_c;
	bool m_enabled;
	Ogre::OverlayElement* m_pText;
	Ogre::PanelOverlayElement* m_pContainer;
	Ogre::String m_text;
};

