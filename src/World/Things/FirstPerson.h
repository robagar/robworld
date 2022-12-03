#pragma once
#include "MovableThing.h"

class FirstPerson : public MovableThing
{
	Ogre::Camera* m_pCamera;

	Ogre::Vector3 m_speeds;

  Thing* m_pFollowTarget;
  bool m_following;

public:
	FirstPerson(const Config& config, Ogre::Camera* pCamera);

	Ogre::Camera* GetCamera() { return m_pCamera; }
  void SetCamera(Ogre::Camera* pCamera) { m_pCamera = pCamera; }
	__declspec(property(get=GetCamera, put=SetCamera)) Ogre::Camera* Camera;

	// MovableThing functions
	void Move(float deltaTime);

  void SetDirection(const Ogre::Vector3& dir);

	float GetFarClipDistance() { return m_pCamera->getFarClipDistance(); }

  //virtual std::string getDebugText();

  // motion
  void yaw(const Ogre::Radian&);
  void yawWorldUp(const Ogre::Radian&);
  void pitch(const Ogre::Radian&);
  void roll(const Ogre::Radian&);

  void startFollowing(Thing* pTarget);
  void stopFollowing();
  void follow();
};
