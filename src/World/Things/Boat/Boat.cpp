#include "stdafx.h"
#include "RigidThing.h"
#include "SoftThing.h"
#include "Float.h"
#include "Rudder.h"
#include "Keel.h"
#include "Boom.h"
#include "Sail.h"
#include "Flag.h"
#include "Boat.h"
#include "Physics.h"

using namespace Ogre;

Boat::Boat(const string& name)
  : RigidThing(name, "Hull.mesh"),
    m_pRudder(NULL),
    m_pKeel(NULL),
    m_pBoom(NULL),
    m_pMainsail(NULL),
    m_pFlag(NULL)
{
  setRigidBody(RigidBodyProperties(new btCylinderShape(btVector3(1.0f, 4.0f, 0.5f)), 30.0f));
  RigidBody->setDamping(0.1f, 0.9f);

  m_pMast = createChild("mast", "Mast.mesh", Vector3(0, 1, 0));

  initRudder();
  initKeel();
  initBoom();
  initMainsail();
  //initFlag();

  initFloats();
}

void Boat::initRudder()
{
  m_pRudder = new ::Rudder("Rudder", 1.0f);
  m_pRudder->attachToBoat(this);
}

void Boat::initKeel()
{
  m_pKeel = new Keel("keel", 30.0f);
  attachChild(m_pKeel);
  m_pKeel->LocalPosition = Vector3(0, 0, -1.1f);

  btTransform frameInBoat;
  frameInBoat.setIdentity();
  frameInBoat.setOrigin(ogre2bullet(m_pKeel->LocalPosition));
  
  btTransform frameInKeel;
  frameInKeel.setIdentity();

  btGeneric6DofConstraint* pConstraint = new btGeneric6DofConstraint(*RigidBody,
                                                                     *m_pKeel->RigidBody,
                                                                     frameInBoat,
                                                                     frameInKeel, 
                                                                     true);
  for(int i=0; i<6; ++i)
    pConstraint->setLimit(i, 0, 0);

  Physics::instance()->m_pDynamicsWorld->addConstraint(pConstraint, true);
      
}

void Boat::initBoom()
{
  m_pBoom = new ::Boom("boom", 1.0f);
  m_pBoom->attachToBoat(this);
}

void Boat::initMainsail()
{
  Vector3 head(0.0f, 0.8f, 9.0f);
  Vector3 tack(0.0f, 0.8f, 1.0f);
  Vector3 clew(0.0f, -3.5f, 1.0f);
  m_pMainsail = new Sail("mainsail", head, tack, clew);
  m_pMainsail->attachToBoat(this, this->m_pBoom);
}

void Boat::initFlag()
{
  Vector3 mastTop(0.0f, 1.0f, 10.0f);
  m_pFlag = new Flag("flag", mastTop, 1.0f, 2.0f);
  m_pFlag->attachToBoat(this);
}

void Boat::initFloats()
{
//  addFloat(new Float(name + "-center", 2.0f, 20.0f));
  addFloat(new Float(m_name + "-bow", 1.0f, 40.0f, Vector3(0.0f, 5.0f, 0.0f)));
  addFloat(new Float(m_name + "-stern", 1.0f, 40.0f, Vector3(0.0f, -5.0f, 0.0f)));
  addFloat(new Float(m_name + "-port", 1.0f, 20.0f, Vector3(-2.0f, 0.0f, 0.0f)));
  addFloat(new Float(m_name + "-starboard", 1.0f, 20.0f, Vector3(2.0f, 0.0f, 0.0f)));}

void Boat::blow(const Vector3& windVelocity)
{
  if (m_pMainsail)
    m_pMainsail->blow(windVelocity);

  if (m_pFlag)
    m_pFlag->blow(windVelocity);
}

string Boat::getDebugText()
{
  ostringstream o;

  //o << "Direction: " << Direction << endl;
  o << "Heading: " << fixed << setprecision(1) << Heading << endl;
  o << "Speed: " << fixed << setprecision(1) << Speed << endl;

  //o << "Boom angle: " << setprecision(3) << m_pBoom->Angle.valueDegrees() << endl;
  //o << "Boom max angle: " << setprecision(3) << m_pBoom->MaxAngle.valueDegrees() << endl;
  //o << "Rudder angle: " << fixed << setprecision(1) << m_pRudder->Angle.valueDegrees() << endl;

  return o.str();
}