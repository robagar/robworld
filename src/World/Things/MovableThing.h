#pragma once
#include "Thing.h"

class MovableThing : public Thing
{
protected:
	/**
	* Lat/long position.
	*/
	Geo::GeoLocation m_location;

	/**
	* Velocity component in local coordinates (ie negative z points forwards regardless of orientation)
	*/
	Ogre::Vector3 m_localVelocity;

	/**
	* Velocity component in world space
	*/
	Ogre::Vector3 m_worldVelocity;

	Ogre::Radian m_yawAngle;
	float m_yawRate;
	float m_maxYawRate;

	Ogre::Radian m_pitchAngle;
	float m_pitchRate;
	float m_maxPitchRate;

	Ogre::Radian m_rollAngle;

	MovableThing(const std::string& name);

public:
	virtual ~MovableThing();

	virtual bool IsMovable() { return true; }


	void Stop();

	void AddWorldVelocity(Ogre::Vector3 v) { m_worldVelocity += v; }

	Ogre::Vector3& GetLocalVelocity() { return m_localVelocity; }
	void SetLocalVelocity(Ogre::Vector3 v) { m_localVelocity = v; }
	void AddLocalVelocity(Ogre::Vector3 v) { m_localVelocity += v; }

//	void Pitch(float rate) { m_pitchAngle = m_maxPitchRate * rate; } 
	void AddPitch(Ogre::Radian pitch) { m_pitchAngle += pitch; }
	void AddPitchRate(float pitchRate) { m_pitchRate += pitchRate; }

	void SetRollAngle(Ogre::Radian rollAngle) { m_rollAngle = rollAngle; }

//	void Yaw(float rate) { m_yaw = m_maxYawRate * rate; } 
	void AddYaw(Ogre::Radian yaw) { m_yawAngle += yaw; }
	void AddYawRate(float yawRate) { m_yawRate += yawRate; }

	virtual void Move(float deltaTime) PURE;
};

typedef std::vector<MovableThing*> MovableThingVec;
typedef MovableThingVec::iterator MovableThingVecIt;