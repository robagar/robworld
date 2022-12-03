#include "../StdAfx.h"
#include "Commands.h"
#include "../World/Things/Thing.h"
#include "../World/Things/MovableThing.h"
#include "../View/WorldView.h"
#include "../World/World.h"

SystemCommandTarget* SystemCommand::s_pTarget;

void NextViewState::Execute()
{
	s_pTarget->SetNextViewState();
}

void TakeScreenshot::Execute()
{
	s_pTarget->TakeScreenshot();
}

void ToggleMetaCam::Execute()
{
	s_pTarget->ToggleMetaCam();
}


namespace Commands
{
	void Stop::Execute()
	{
		((MovableThing*)pSubject)->Stop();
	}

	void MoveAtConstantVelocity::Execute()
	{
		((MovableThing*)pSubject)->AddLocalVelocity(Velocity);
	}

	void AddWorldVelocity::Execute()
	{
		((MovableThing*)pSubject)->AddWorldVelocity(Velocity);
	}


	void Yaw::Execute()
	{
		((MovableThing*)pSubject)->AddYawRate(Rate);
	}

	void YawWithRoll::Execute()
	{
		((MovableThing*)pSubject)->AddYawRate(Rate);
	}

	void AddYaw::Execute()
	{
		((MovableThing*)pSubject)->AddYaw(Radians);
	}

	void Pitch::Execute()
	{
		((MovableThing*)pSubject)->AddPitchRate(Rate);
	}

	void AddPitch::Execute()
	{
		((MovableThing*)pSubject)->AddPitch(Radians);
	}

	void TurnRight::Execute()
	{
		((MovableThing*)pSubject)->AddYawRate(Rate);
	}

  void ThrowThing::Execute()
  {
    pSubject->throwThing(pObject, speed);
  }

  void Blow::Execute()
  {
    pSubject->blow(velocity);
    //World::instance()->m_enableWind = !World::instance()->m_enableWind;
  }
}