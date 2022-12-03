#include "../../StdAfx.h"
#include "Thing.h"
#include "RigidThing.h"
#include "../../Physics/ThingBulletMotionState.h"
#include "../../Physics/Float.h"
#include "ThingMaker.h"
//#include "Boat/Boat.h"

using namespace Ogre;

SceneManager* ThingMaker::s_pSceneManager;

void ThingMaker::init(SceneManager* pSceneManager)
{
  s_pSceneManager = pSceneManager;
}

Thing* ThingMaker::staticSphere(const String& materialName)
{
  static int n = 0;

  std::ostringstream o;
  o << "static-sphere-" << n++;
  String name = o.str();

  Thing* pThing = new Thing(name, "Ball.mesh", materialName);
  return pThing;
}


Thing* ThingMaker::ball()
{
  static int n = 0;

  std::ostringstream o;
  o << "ball-" << n++;
  String name = o.str();

  RigidThing* pThing = new RigidThing(name, "Ball.mesh");
  pThing->setRigidBody(RigidBodyProperties(new btSphereShape(0.5f), 1.0f));
  pThing->addFloat(new Float(name, 0.5f, 2.0f)); 

  return pThing;
}

//Boat* ThingMaker::boat()
//{
//  static int n = 0;
//
//  std::ostringstream o;
//  o << "boat-" << n++;
//  String name = o.str();
//
//  return new Boat(name);
//}

Thing* ThingMaker::lineMarkers(const String& name, const Vector3& start, const Vector3& delta, int length, const String& materialName)
{
  Thing* pLine = new Thing(name);

  Thing* pMarker;
  for (int i=0; i<length; ++i)
  {
    pMarker = ThingMaker::staticSphere(materialName);
    pMarker->WorldPosition = start + (Real)i * delta;

    if (i % 10 == 0)
      pMarker->Scale = 2.0f;

    pLine->attachChild(pMarker);
  }

  return pLine;
}

Thing* ThingMaker::worldAxes(int length, Real scale)
{
  Thing* pAxes = new Thing("world-axes");

  pAxes->attachChild(lineMarkers("positive-x-axis", Vector3::ZERO, scale * Vector3::UNIT_X,          length, "red"));
  pAxes->attachChild(lineMarkers("negative-x-axis", Vector3::ZERO, scale * Vector3::NEGATIVE_UNIT_X, length, "red"));

  pAxes->attachChild(lineMarkers("positive-y-axis", Vector3::ZERO, scale * Vector3::UNIT_Y,          length, "green"));
  pAxes->attachChild(lineMarkers("negative-y-axis", Vector3::ZERO, scale * Vector3::NEGATIVE_UNIT_Y, length, "green"));

  pAxes->attachChild(lineMarkers("positive-z-axis", Vector3::ZERO, scale * Vector3::UNIT_Z,          length, "blue"));
  pAxes->attachChild(lineMarkers("negative-z-axis", Vector3::ZERO, scale * Vector3::NEGATIVE_UNIT_Z, length, "blue"));

  return pAxes;
}