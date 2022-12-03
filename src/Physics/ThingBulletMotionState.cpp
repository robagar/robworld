#include "../StdAfx.h"
#include "BulletUtil.h"
#include "../World/Things/Thing.h"
#include "ThingBulletMotionState.h"

ThingBulletMotionState::ThingBulletMotionState(Thing* pThing)
  :m_pThing(pThing)
{
}

void ThingBulletMotionState::getWorldTransform(btTransform &worldTrans) const
{
  Ogre::Quaternion orientation = m_pThing->WorldOrientation;
  worldTrans.setRotation(ogre2bullet(orientation));

  Ogre::Vector3 position = m_pThing->WorldPosition;
  worldTrans.setOrigin(ogre2bullet(position));
}

void ThingBulletMotionState::setWorldTransform(const btTransform &worldTrans)
{
  btQuaternion orientation = worldTrans.getRotation();
  m_pThing->WorldOrientation = bullet2ogre(orientation);

  btVector3 position = worldTrans.getOrigin();
  m_pThing->WorldPosition = bullet2ogre(position);
}
