#pragma once

#include "../../World/World.h"

class HUDElement
{
protected:
	World* m_pWorld;

	HUDElement(World* pWorld) 
		: m_pWorld(pWorld)
	{}

public:
	virtual ~HUDElement() {}
	virtual void Update() PURE;
};

