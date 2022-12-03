#pragma once

#include <OIS.h>
#include "InputState.h"
struct Command;
class MovableThing;

struct JoystickAxisParams
{
	int Axis;
	float Multiplier;
	float Threshold;

	JoystickAxisParams(int axis, float multiplier, float threshold = 0.05f);
};

struct InputParams
{
	float MouseLookRadiansPerPixel;

	JoystickAxisParams JoystickPitch;
	JoystickAxisParams JoystickRoll;
	JoystickAxisParams JoystickYaw;
	JoystickAxisParams JoystickThrottle;

	unsigned int KeyPressMinInterval;

	InputParams();
};

class Input 
{
private:
	bool m_capture;

	InputParams m_params;

	OIS::InputManager* m_pOIS;
	OIS::Mouse* m_pMouse;
	OIS::Keyboard* m_pKeyboard;
	OIS::JoyStick* m_pJoystick;

	InputState m_input;
	std::queue<Command*>& m_commandQueue;
	MovableThing* m_pThingUnderControl;

	Ogre::Timer m_lastKeyPressTimer;

public:
	Input(HWND hWnd, bool capture, bool exclusive, std::queue<Command*>& commandQueue);
	~Input();

	InputState& GetInputState() { return m_input; }

	void SetThingUnderControl(MovableThing* pThingUnderControl) { m_pThingUnderControl = pThingUnderControl; }

	void SetWindowExtents(int width, int height) ;

	void ProcessInput();

	String GetDebugText();

private:
	void InitializeCapture(HWND hWnd, bool exclusive);
	void CaptureInput();

	void CaptureMouseInput();
	void CaptureKeyboardInput();
	void CaptureJoystickInput();

	float GetJoystickAxisValue(const JoystickAxisParams& params);
	int GetJoystickHatDirection();


};