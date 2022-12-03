#pragma once

class Boat;

class ThingMaker
{
  static Ogre::SceneManager* s_pSceneManager;


public:
  static void init(Ogre::SceneManager*);

  static Thing* ball();
  static Thing* staticSphere(const String& materialName = "default");

  static Boat* boat();

  static Thing* lineMarkers(const String& name, const Vector3& start, const Vector3& delta, int length, const String& materialName = "default");

  static Thing* worldAxes(int length, Real scale = 10.0f);
};