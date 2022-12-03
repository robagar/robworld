#pragma once

class Thing;

class ThingBulletMotionState : public btMotionState
{
  Thing* m_pThing;

public:
  ThingBulletMotionState(Thing*);

  virtual void getWorldTransform(btTransform &worldTrans) const;
  virtual void setWorldTransform(const btTransform &worldTrans);
};