#include "../../StdAfx.h"
#include "Sky.h"


using namespace Ogre;

Sky::Sky(Ogre::SceneManager* pSceneManager, Ogre::Camera* pCamera)
: m_pSceneManager(pSceneManager),
  m_pCamera(pCamera)
{
	Quaternion orientation;
	orientation.FromAngleAxis(Radian(Math::HALF_PI), Vector3::UNIT_X);
	m_pSceneManager->setSkyBox(true, "ProceduralSkyMaterial", 10, true, orientation);
}

void Sky::update(Real time, Real deltaTime)
{
}