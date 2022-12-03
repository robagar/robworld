#pragma once

class Thing;

class WorldListener
{
public:
	virtual void ThingAddedToWorld(Thing* pThing) PURE;
	virtual void WorldResized(const Ogre::AxisAlignedBox& worldBoundingBox) PURE;
};
