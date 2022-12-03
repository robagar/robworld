#pragma once

class SystemCommandTarget;
class Thing;
class MovableThing;

enum CommandType
{
	CommandType_System,
	CommandType_Control
};

struct Command
{
	CommandType Type;

	bool IsSystemCommand() { return Type == CommandType_System; }

protected:

public:
	virtual void Execute() PURE;
};

struct SystemCommand : public Command
{
	static void SetTarget(SystemCommandTarget* pTarget) { s_pTarget = pTarget; }

protected:
	static SystemCommandTarget* s_pTarget;

	SystemCommand()
	{
		Type = CommandType_System;
	}
};

struct NextViewState : public SystemCommand
{
public:
	void Execute();
};


struct TakeScreenshot : public SystemCommand
{
public:
	void Execute();
};

struct ToggleMetaCam : public SystemCommand
{
public:
	void Execute();
};


//struct QuitCommand : public SystemCommand
//{
//};

struct ControlCommand : public Command
{
	Thing* pSubject;

protected:
	ControlCommand(Thing* _pSubject)
		: pSubject(_pSubject)
	{
		Type = CommandType_Control;
	}
};

namespace Commands
{
	struct Stop : public ControlCommand
	{
		Stop(MovableThing* _pSubject)
			: ControlCommand((Thing*)_pSubject)
		{
		}

		void Execute();
	};

	struct MoveAtConstantVelocity : public ControlCommand
	{
		Ogre::Vector3 Velocity;

		MoveAtConstantVelocity(MovableThing* _pSubject, Ogre::Vector3 velocity)
			: ControlCommand((Thing*)_pSubject),
				Velocity(velocity)
		{
		}

		void Execute();
	};


	struct AddWorldVelocity : public ControlCommand
	{
		Ogre::Vector3 Velocity;

		AddWorldVelocity(MovableThing* _pSubject, Ogre::Vector3 velocity)
			: ControlCommand((Thing*)_pSubject),
				Velocity(velocity)
		{
		}

		void Execute();
	};

	struct Yaw : public ControlCommand
	{
		float Rate;

		Yaw(MovableThing* _pSubject, float rate)
			: ControlCommand((Thing*)_pSubject),
				Rate(rate)
		{
		}

		void Execute();
	};

	struct YawWithRoll : public ControlCommand
	{
		float Rate;

		YawWithRoll(MovableThing* _pSubject, float rate)
			: ControlCommand((Thing*)_pSubject),
				Rate(rate)
		{
		}

		void Execute();
	};

	struct AddYaw : public ControlCommand
	{
		Ogre::Radian Radians;

		AddYaw(MovableThing* _pSubject, Ogre::Radian radians)
			: ControlCommand((Thing*)_pSubject),
				Radians(radians)
		{
		}

		void Execute();
	};

	struct Pitch : public ControlCommand
	{
		float Rate;

		Pitch(MovableThing* _pSubject, float rate)
			: ControlCommand((Thing*)_pSubject),
				Rate(rate)
		{
		}

		void Execute();
	};

	struct AddPitch : public ControlCommand
	{
		Ogre::Radian Radians;

		AddPitch(MovableThing* _pSubject, Ogre::Radian radians)
			: ControlCommand((Thing*)_pSubject),
				Radians(radians)
		{
		}

		void Execute();
	};

	struct TurnRight : public ControlCommand
	{
		float Rate;

		TurnRight(MovableThing* _pSubject, float rate)
			: ControlCommand((Thing*)_pSubject),
				Rate(rate)
		{
		}

		void Execute();
	};

  struct ThrowThing : public ControlCommand
  {
    Thing* pObject;
    Real speed;

    ThrowThing(Thing* _pSubject, Thing* _pObject, Real _speed)
			: ControlCommand(_pSubject),
        pObject(_pObject),
        speed(_speed)
    {
    }

    void Execute();
  };

  struct Blow : public ControlCommand
  {
    Vector3 velocity;

    Blow(Thing* _pSubject, Vector3 _velocity)
			: ControlCommand(_pSubject),
        velocity(_velocity)
    {
    }

    void Execute();
  };
}