#include "../../StdAfx.h"
#include "MovableThing.h"

using namespace Ogre;
using namespace Geo;

MovableThing::MovableThing(const String& name)
: Thing(name), 
	m_localVelocity(Vector3::ZERO),
	m_worldVelocity(Vector3::ZERO)
{
}

MovableThing::~MovableThing()
{
}

void MovableThing::Stop()
{
	m_localVelocity = 0;
	m_worldVelocity = 0;
	m_yawAngle = 0;
	m_yawRate = 0;
	m_pitchAngle = 0;
	m_pitchRate = 0;
}
