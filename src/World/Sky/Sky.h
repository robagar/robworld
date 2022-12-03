#pragma once

class Sky
{
	Ogre::SceneManager* m_pSceneManager;
  Ogre::Camera* m_pCamera;

public:
	Sky(Ogre::SceneManager* pSceneManager, Ogre::Camera* pCamera);

  void update(Ogre::Real time, Ogre::Real deltaTime);
};