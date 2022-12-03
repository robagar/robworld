#pragma once
#include "HUDElement.h"

template <class T> class SingleOverlayElementHUDElement : public HUDElement
{
protected:
	T* m_pOverlayElement;

public:
	SingleOverlayElementHUDElement(World* pWorld, T* pOverlayElement)
		: HUDElement(pWorld),
			m_pOverlayElement(pOverlayElement) {}
};

