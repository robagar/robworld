#pragma once

#include "RigidThing.h"

class Rudder;
class Keel;
class Boom;
class Sail;
class Flag;

class Boat : public RigidThing
{
  Thing* m_pMast;

  Rudder* m_pRudder;
  Keel* m_pKeel;
  Boom* m_pBoom;
  Sail* m_pMainsail;
  Flag* m_pFlag;

public:
  Boat(const string& name);

  Rudder& getRudder() { return *m_pRudder; }
  __declspec(property(get=getRudder)) Rudder& Rudder;

  Boom& getBoom() { return *m_pBoom; }
  __declspec(property(get=getBoom)) Boom& Boom;

  virtual void blow(const Ogre::Vector3& windVelocity);

  virtual std::string getDebugText();

private:
  void initRudder();
  void initKeel();
  void initBoom();
  void initMainsail();
  void initFlag();
  void initFloats();

};
