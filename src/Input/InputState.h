#pragma once

struct InputState
{
	bool Shutdown;
	
	Real MoveForwards;
	Real MoveRight;
	Real MoveUp;
	
	bool MoveFast;

	Real TurnRight;
	
	Real Pitch;
	Real Roll;
	Real Yaw;

	bool EnableMouseLook;
	Real MouseDeltaX;
	Real MouseDeltaY;
	Real MouseX;
	Real MouseY;
	Real MouseNormalizedX;
	Real MouseNormalizedY;

  bool Action;

  Real Rudder;
  Real Mainsheet;

	bool Screenshot;
	bool IncreaseLOD;
	bool DecreaseLOD;

	bool NextViewState;
  bool ToggleMetaCam;

	InputState()
	{
		Clear();
	}

	void Clear()
	{
		ZeroMemory(this, sizeof(InputState));
	}
};