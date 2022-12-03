#include "../StdAfx.h"
#include <OgreStringConverter.h>
using namespace Ogre;

#include "Input.h"
#include "../Command/Commands.h"
//#include "../Command/RudderCommand.h"
//#include "../Command/MainsheetCommand.h"
#include "../World/Things/Thing.h"
#include "../World/Things/ThingMaker.h"
#include "../World/World.h"

JoystickAxisParams::JoystickAxisParams(int axis, float multiplier, float threshold)
: Axis(axis), 
	Multiplier(multiplier),
	Threshold(threshold)
{
}

InputParams::InputParams()
: MouseLookRadiansPerPixel(0.007f),
	JoystickPitch(0, 0.1f),
	JoystickRoll(1, -0.2f),
	JoystickYaw(2, -0.2f),
	JoystickThrottle(3, 3.0f),
	KeyPressMinInterval(500)
{
}

Input::Input(HWND hWnd, bool capture, bool exclusive, std::queue<Command*>& commandQueue)
: m_pOIS(NULL), 
	m_pMouse(NULL),
	m_pKeyboard(NULL),
	m_pJoystick(NULL),
	m_capture(capture),
	m_commandQueue(commandQueue),
  m_pThingUnderControl(NULL)
{
	if (capture)
		InitializeCapture(hWnd, exclusive);
}

void Input::InitializeCapture(HWND hWnd, bool exclusive)
{
	OIS::ParamList pl;
	pl.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString((unsigned long)hWnd)));

	// by default OIS takes exclusive control of the mouse
	if (!exclusive)
	{
		#if defined OIS_WIN32_PLATFORM
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
		pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
		pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
		pl.insert(std::make_pair(std::string("w32_joystick"), std::string("DISCL_BACKGROUND")));
		pl.insert(std::make_pair(std::string("w32_joystick"), std::string("DISCL_NONEXCLUSIVE")));
		#elif defined OIS_LINUX_PLATFORM
		pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
		pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
		pl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
		pl.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
		#endif
	}

	m_pOIS = OIS::InputManager::createInputSystem( pl );
	m_pMouse = static_cast<OIS::Mouse*>(m_pOIS->createInputObject(OIS::OISMouse, true ));
	m_pKeyboard = static_cast<OIS::Keyboard*>(m_pOIS->createInputObject(OIS::OISKeyboard, true));
	
	//if (m_pOIS->getNumberOfDevices(OIS::OISJoyStick) != 0)
	//	m_pJoystick = static_cast<OIS::JoyStick*>(m_pOIS->createInputObject(OIS::OISJoyStick, true));
}

Input::~Input() 
{
	//delete m_pMouse;
	//delete m_pKeyboard;
	//delete m_pJoystick;
	OIS::InputManager::destroyInputSystem(m_pOIS);
}

void  Input::SetWindowExtents(int width, int height)
{
	//Set Mouse Region.. if window resizes, we should alter this to reflect as well
	const OIS::MouseState &ms = m_pMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

void Input::CaptureInput()
{
	// clear all input
	m_input.Clear();

	CaptureMouseInput();
	CaptureKeyboardInput();
	if (m_pJoystick)
		CaptureJoystickInput();
}

void Input::CaptureMouseInput()
{
	m_pMouse->capture();
	const OIS::MouseState& mouseState = m_pMouse->getMouseState();

	m_input.MouseDeltaX = (Real)mouseState.X.rel; 
	m_input.MouseX = (Real)mouseState.X.abs;
	m_input.MouseNormalizedX = (Real)m_input.MouseX / mouseState.width;

	m_input.MouseDeltaY = (Real)mouseState.Y.rel;
	m_input.MouseY = (Real)mouseState.Y.abs;
	m_input.MouseNormalizedY = (Real)m_input.MouseY / mouseState.height;

	m_input.EnableMouseLook = mouseState.buttonDown(OIS::MB_Right);
}

void Input::CaptureKeyboardInput()
{
	// get device states
	m_pKeyboard->capture();
	
	if (m_pKeyboard->isKeyDown(OIS::KC_UP) || m_pKeyboard->isKeyDown(OIS::KC_W))
		m_input.MoveForwards = 1.0f;
	else if	(m_pKeyboard->isKeyDown(OIS::KC_DOWN) || m_pKeyboard->isKeyDown(OIS::KC_S))
		m_input.MoveForwards = -1.0f;
	else 
		m_input.MoveForwards = 0.0f;
	
	if (m_pKeyboard->isKeyDown(OIS::KC_D))
		m_input.MoveRight = 1.0f;
	else if (m_pKeyboard->isKeyDown(OIS::KC_A))
		m_input.MoveRight = -1.0f;
	else
		m_input.MoveRight = 0.0f;

	if (m_pKeyboard->isKeyDown(OIS::KC_R))
		m_input.MoveUp = 1.0f; 
	else if (m_pKeyboard->isKeyDown(OIS::KC_F))
		m_input.MoveUp = -1.0f;
	else
		m_input.MoveUp = 0.0f;
	
	m_input.MoveFast = m_pKeyboard->isKeyDown(OIS::KC_LSHIFT);

	if (m_pKeyboard->isKeyDown(OIS::KC_RIGHT))
		m_input.TurnRight = 1.0f;
	else if (m_pKeyboard->isKeyDown(OIS::KC_LEFT))
		m_input.TurnRight = -1.0f;
	else
		m_input.TurnRight = 0.0f;

  // boat rudder
  if (m_pKeyboard->isKeyDown(OIS::KC_PERIOD))
    m_input.Rudder = 1.0f;
  if (m_pKeyboard->isKeyDown(OIS::KC_COMMA))
    m_input.Rudder = -1.0f;

  // boat mainsheet
  if (m_pKeyboard->isKeyDown(OIS::KC_L))
    m_input.Mainsheet = 1.0f;
  if (m_pKeyboard->isKeyDown(OIS::KC_K))
    m_input.Mainsheet = -1.0f;


	// key down flags 
//	m_input.EnableMouseLook = !(m_pKeyboard->isKeyDown(OIS::KC_RCONTROL) || m_pKeyboard->isKeyDown(OIS::KC_LCONTROL));
//#ifdef _DEBUG
//  m_input.EnableMouseLook = !m_input.EnableMouseLook;
//#endif

  m_input.Action = m_pKeyboard->isKeyDown(OIS::KC_SPACE);

	// single shot key presses
	if (m_lastKeyPressTimer.getMilliseconds() > m_params.KeyPressMinInterval)
	{
		m_lastKeyPressTimer.reset();

//    m_input.Action = m_pKeyboard->isKeyDown(OIS::KC_SPACE);

		m_input.Shutdown = m_pKeyboard->isKeyDown(OIS::KC_ESCAPE);
		m_input.Screenshot = m_pKeyboard->isKeyDown(OIS::KC_E);
		m_input.IncreaseLOD = m_pKeyboard->isKeyDown(OIS::KC_EQUALS);
		m_input.DecreaseLOD = m_pKeyboard->isKeyDown(OIS::KC_MINUS);

		m_input.NextViewState = m_pKeyboard->isKeyDown(OIS::KC_F2);
    m_input.ToggleMetaCam = m_pKeyboard->isKeyDown(OIS::KC_F3);
	}

}

void Input::CaptureJoystickInput()
{
	m_pJoystick->capture();

	// NB: keyboard input takes precedence, so only use joystick if input state values are zero 

	// joystick controls pitch and yaw
	m_input.Pitch = GetJoystickAxisValue(m_params.JoystickPitch);
	m_input.Roll = GetJoystickAxisValue(m_params.JoystickRoll);
	m_input.Yaw = GetJoystickAxisValue(m_params.JoystickYaw);

	// use throttle to move forwards/backwards
	if (m_input.MoveForwards == 0)
	{
		float throttle = -GetJoystickAxisValue(m_params.JoystickThrottle);
		m_input.MoveForwards = throttle * throttle * Math::Sign(throttle); 
	}

	// use hat to strafe
	int hatDirection = GetJoystickHatDirection();
	if (hatDirection != OIS::Pov::Centered && m_input.MoveRight == 0)
	{
		if (hatDirection & OIS::Pov::East)
			m_input.MoveRight = 1.0f;
		else if (hatDirection & OIS::Pov::West)
			m_input.MoveRight = -1.0f;

		if (hatDirection & OIS::Pov::North)
			m_input.MoveUp = 1.0f;
		else if (hatDirection & OIS::Pov::South)
			m_input.MoveUp = -1.0f;
	}
}

float Input::GetJoystickAxisValue(const JoystickAxisParams& params)
{
	if (m_pJoystick == NULL)
		return 0.0f;

	// get integral raw value
	int i = m_pJoystick->getJoyStickState().mAxes[params.Axis].abs;

	// map to real value between -1.0 and 1.0
	float f = i == 0 ? 0 : i < 0 ? (-(float)i) / OIS::JoyStick::MIN_AXIS : ((float)i) / OIS::JoyStick::MAX_AXIS;
	
	// return zero if smaller than the threshold, otherwise the value scaled by the multiplier
	return Math::Abs(f) < params.Threshold ? 0 : f * params.Multiplier;
}

int Input::GetJoystickHatDirection()
{
	if (m_pJoystick->getNumberOfComponents(OIS::OIS_POV) == 0)
		return OIS::Pov::Centered;
	else
		return m_pJoystick->getJoyStickState().mPOV[0].direction; 
}

void Input::ProcessInput()
{
	// get device states
	if (m_capture)
		CaptureInput();

	// only issue commands if something is being controlled by user input
	if (m_pThingUnderControl == NULL)
		return;
	
	// by default, stop moving
	m_commandQueue.push(new Commands::Stop(m_pThingUnderControl));

	// forwards / backwards
	if (m_input.MoveForwards != 0)
		m_commandQueue.push(new Commands::MoveAtConstantVelocity(m_pThingUnderControl, Thing::FORWARDS * m_input.MoveForwards)); 

	// strafe left / right
	if (m_input.MoveRight != 0)
		m_commandQueue.push(new Commands::MoveAtConstantVelocity(m_pThingUnderControl, Thing::RIGHT * m_input.MoveRight)); 

	// up / down
	if (m_input.MoveUp != 0)
		m_commandQueue.push(new Commands::AddWorldVelocity(m_pThingUnderControl, World::UP * m_input.MoveUp)); 

	// rotate left / right
	if (m_input.TurnRight != 0)
		m_commandQueue.push(new Commands::TurnRight(m_pThingUnderControl, m_input.TurnRight)); 

	// pitch
	if (m_input.Pitch != 0)
		m_commandQueue.push(new Commands::Pitch(m_pThingUnderControl, m_input.Pitch));

	// roll
	if (m_input.Roll != 0)
		m_commandQueue.push(new Commands::YawWithRoll(m_pThingUnderControl, m_input.Roll));

	// yaw
	if (m_input.Yaw != 0)
		m_commandQueue.push(new Commands::Yaw(m_pThingUnderControl, m_input.Yaw));

	// mouselook
	if (m_input.EnableMouseLook)
	{
		if (m_input.MouseDeltaX != 0)
			m_commandQueue.push(new Commands::AddYaw(m_pThingUnderControl, Radian(-m_input.MouseDeltaX * m_params.MouseLookRadiansPerPixel)));
		if (m_input.MouseDeltaY != 0)
			m_commandQueue.push(new Commands::AddPitch(m_pThingUnderControl, Radian(-m_input.MouseDeltaY * m_params.MouseLookRadiansPerPixel)));
	}

  //// boat rudder (TODO proper angular speed config)
  //m_commandQueue.push(new RudderCommand(World::instance()->m_pBoat, m_input.Rudder * 0.6366f));

  //// boat mainseet (TODO proper speed config)
  //m_commandQueue.push(new MainsheetCommand(World::instance()->m_pBoat, m_input.Mainsheet * 0.6366f));


  // action (TODO)
//  if (m_input.Action)
//    m_commandQueue.push(new Commands::ThrowThing((Thing*)m_pThingUnderControl, ThingMaker::sphere(), 25.0f));  
//    m_commandQueue.push(new Commands::Blow((Thing*)World::instance()->m_pBoat, 5.0f * m_pThingUnderControl->Direction));  

	if (m_input.NextViewState)
		m_commandQueue.push(new NextViewState());

	if (m_input.Screenshot)
		m_commandQueue.push(new TakeScreenshot());

	if (m_input.ToggleMetaCam)
		m_commandQueue.push(new ToggleMetaCam());

	//if (m_input.IncreaseLOD)
	//	m_commandQueue.push(new Commands::IncreaseLOD());

	//if (m_input.DecreaseLOD)
	//	m_commandQueue.push(new Commands::DecreaseLOD());
}

String Input::GetDebugText()
{
	std::ostringstream o;
	o << "pitch: " << GetJoystickAxisValue(m_params.JoystickPitch) << endl;
	o << "roll: " << GetJoystickAxisValue(m_params.JoystickRoll) << endl;
	o << "yaw: " << GetJoystickAxisValue(m_params.JoystickYaw) << endl;
	return o.str();
}