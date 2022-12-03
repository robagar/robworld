#include "StdAfx.h"
#include "RobWorldApp.h"


RobWorldApp::RobWorldApp()
: m_fullscreen(false),
	m_width(1440),
	m_height(900)
{
	EnterState(Started);
	m_view.SetListener(this);
}

RobWorldApp::~RobWorldApp()
{
}

bool RobWorldApp::EnterState(AppState state)
{
	switch (m_state)
	{
		case Destroying:
			// only allow finished state
			if (state != Finished)
			{
				debug << "Unable to change state while destroying, requested state " << state << endl;
				return false;
			}
			break;

		case Finished: 
			// no further state changes allowed
			debug << "Unable to change state, application finished." << endl;
			return false;
	}

	m_state = state;
	debug << "Application state -> " << GetStateName(state) << endl; 
	return true;
}

String RobWorldApp::GetStateName(AppState state)
{
	switch (state)
	{
		case Started: return "Started";
		case Initializing: return "Initializing";
		case Running: return "Running";
		case Destroying: return "Destroying";
		case Finished: return "Finished";
		default: return "*** Unknown State ***";
	}
}

void RobWorldApp::Run()
{
	Initialize();
	MainLoop();
	Destroy();
	EnterState(Finished);
}

void RobWorldApp::Initialize()
{
	EnterState(Initializing);
	debug << "Initializing app" << endl;
	debug << "Fullscreen: " << std::boolalpha << m_fullscreen << endl;
	debug << "Width: " << m_width << endl;
	debug << "Height: " << m_height << endl;

	m_view.Initialize("RobWorld", m_width, m_height, m_fullscreen);
}

void RobWorldApp::MainLoop()
{
	debug << "Entering main loop" << endl;
	
	EnterState(Running);
	m_view.loop();

	debug << "Exiting main loop" << endl;
}

void RobWorldApp::Destroy()
{
	EnterState(Destroying);

	m_view.StopRenderLoop();
	debug << "Destroying app" << endl;
}

void RobWorldApp::RenderWindowClosed()
{
}

void RobWorldApp::FrameStarted()
{
}

void RobWorldApp::FrameEnded()
{
}

