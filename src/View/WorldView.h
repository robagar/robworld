#pragma once

#include "WorldViewState.h"
#include "../Input/InputState.h"
#include "../World/WorldListener.h"

namespace Ogre
{
	class Root;
	class RenderSystem;
	class RenderWindow;
	class SceneManager;
	class Camera;
}

// classes
class HeadUpDisplay;
class Input;
class World;
class Thing;
class FirstPerson;
struct Command;

//namespace UserData
//{
//	class UserDataManager;
//}


/**
 * WorldView events listener interface.
 */
class WorldViewListener
{
public:
	virtual void RenderWindowClosed() PURE;

	virtual void FrameStarted() PURE;
	virtual void FrameEnded() PURE;
};

/**
 * Functions called by system commands.
 */
class SystemCommandTarget
{
public:
	virtual void Shutdown() PURE;
	virtual void SetNextViewState() PURE;
	virtual void TakeScreenshot() PURE;
  virtual void ToggleMetaCam() PURE;
};

/**
 * 
 */
class WorldView :
	public SystemCommandTarget,
	//public Ogre::FrameListener,
	public Ogre::WindowEventListener,
	public WorldListener 
{
  static WorldView* s_pInstance;

	Config m_config;

	WorldViewListener* m_pListener;

	Ogre::Root* m_pOgre;
	Ogre::RenderSystem* m_pRenderSystem;
	Ogre::RenderWindow* m_pRenderWindow;
	Ogre::SceneManager* m_pSceneManager;
	Ogre::Camera* m_pCamera;
	Ogre::Camera* m_pMetaCam;
	Ogre::Camera* m_pDummyCam;
	Ogre::Viewport* m_pViewport;

	HeadUpDisplay* m_pHUD;

	Input* m_pInput;

	World* m_pWorld;

	//UserData::UserDataManager* m_pUserData;

	bool m_fullscreen; 

	bool m_exitLoop;

	unsigned int m_frameNumber;

	std::queue<Command*> m_commandQueue;

	WorldViewState* m_pViewState;
	std::map<String, WorldViewState> m_viewStates;


private:


public:
 	WorldView();
	~WorldView();

  static WorldView* instance() { return s_pInstance; }

	void Destroy();

	void SetListener(WorldViewListener* pListener) { m_pListener = pListener; }

	void Initialize(std::string windowTitle, 
									unsigned int width, 
									unsigned int height, 
									bool fullscreen = false, 
									std::string parentWindowHandle = "",
									bool captureInput = true);

  void loop();
	void RenderOneFrame();

	//void StartRenderLoop();
	void StopRenderLoop();

	InputState& GetInputState();

	unsigned int GetFrameNumber() { return m_frameNumber; }

	//const Geo::GeoLocation& GetFirstPersonLocation();

	void EnableFog(bool enable);

	// SystemCommandTarget
	void Shutdown();
	void ToggleWireFrame();
	void TakeScreenshot();

	// Ogre::FrameListener 
	//bool frameStarted(const Ogre::FrameEvent& frame);
	//bool frameRenderingQueued(const Ogre::FrameEvent& frame);
	//bool frameEnded(const Ogre::FrameEvent& frame);

	// Ogre::WindowEventListener
	void windowResized(Ogre::RenderWindow* pRenderWindow);
	bool windowClosing(Ogre::RenderWindow* pRenderWindow);
	void windowClosed(Ogre::RenderWindow* pRenderWindow);
	void windowFocusChange(Ogre::RenderWindow* pRenderWindow);

	// WorldListener
	void ThingAddedToWorld(Thing* pThing);
	void WorldResized(const Ogre::AxisAlignedBox& worldBoundingBox);

  Ogre::Camera* getCamera() { return m_pCamera; }
  __declspec(property(get=getCamera)) Ogre::Camera* Camera;

  static Ogre::SceneNode* createSceneNode(const String& name);

private: 
	void LoadOgrePlugin(std::string pluginName);

	void CreateRenderer();
	void CreateRenderWindow(std::string windowTitle, 
													unsigned int width, 
													unsigned int height, 
													bool fullscreen, 
													String parentWindowHandle);
	void InitializeResources();
	void CreateSceneManager();

	void CreateCamera();
	void CreateHUD();

	void InitializeInput(bool captureInput);

	void CreateWorld();

	//void InitializeUserData();

	void InitializeViewStates();
	void AddViewState(const WorldViewState& viewState);
	void SetNextViewState();
	void SetViewState(String name);
	void SetViewState(WorldViewState*);

  void ToggleMetaCam();

	void Update(float deltaTime);
	void ProcessCommandQueue();

	void UpdateHUD();

	void OnRenderWindowClosed();
	void OnFrameStarted();
	void OnFrameEnded();

};