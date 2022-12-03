#pragma once

#include "View/WorldView.h"

/**
 *
 */
enum AppState
{
	Started,
	Initializing,
	Running,
	Destroying,
	Finished
};

class RobWorldApp : public WorldViewListener
{
	/**
	 * True if running fullscreen.
	 */
	bool m_fullscreen;

	/**
	 * Renderer window width in pixels.
	 */
	unsigned int m_width;

	/**
	 * Renderer window height in pixels.
	 */
	unsigned int m_height;

	/**
	 * The 3D view
	 */
	WorldView m_view;


	/**
	 * The application state.
	 */
	AppState m_state;

public:
	/**
	 * Constructor.
	 */
	RobWorldApp();

	/**
	 * Destructor. 
	 */
	~RobWorldApp();

	/**
	 * Changes the application state.
	 *
	 * The state change may not be possible, eg if the application is shutting down. 
	 *
	 * @param state the new state
	 * @return true if state change succeeded
	 */
	bool EnterState(AppState state);


	/**
	 * Returns the human readable name of an application state
	 * 
	 * @param state
	 * @return the state name
	 */
	std::string GetStateName(AppState state);

	/**
	 * The main application function. 
	 */
	void Run();

	/**
	 * Initializes the application.
	 */
	void Initialize();

	/**
	 * The application execution loop.
	 */
	void MainLoop();

	/**
	 * Releases and tidies up all resources. 
	 */
	void Destroy();

	// ReefViewListener methods
	void RenderWindowClosed();
	void FrameStarted();
	void FrameEnded();

};
