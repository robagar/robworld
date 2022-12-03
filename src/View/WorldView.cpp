#include "../StdAfx.h"

#include "../Misc/Clock.h"
#include "../World/World.h"
#include "WorldView.h"
#include "HUD/HUD.h"
#include "../Input/Input.h"
#include "../World/Things/Thing.h"
#include "../World/Things/ThingMaker.h"
#include "../World/Things/FirstPerson.h"
#include "../Command/Commands.h"

//#include "Globals.h"

//using namespace std;
//using namespace Ogre;
//using namespace Geo;

WorldView* WorldView::s_pInstance = NULL;

WorldView::WorldView()
:	m_pListener(NULL), 
	m_pOgre(NULL),
	m_pHUD(NULL),
	m_pInput(NULL),
	m_pWorld(NULL),
	//m_pUserData(NULL),
	m_pViewState(NULL),
  m_pMetaCam(NULL),
  m_pDummyCam(NULL),
  m_exitLoop(false)
{
  assert(s_pInstance == NULL);
  s_pInstance = this;

  SystemCommand::SetTarget(this);
}

WorldView::~WorldView()
{
	Destroy();
}


void WorldView::Destroy()
{
	delete m_pInput;
	delete m_pWorld;
	delete m_pHUD;
	delete m_pOgre;
}

void WorldView::Shutdown()
{
}

void WorldView::Initialize(String windowTitle, 
													unsigned int width,	unsigned int height, 
													bool fullscreen, 
													String parentWindowHandle,
													bool captureInput)
{
	m_config.LoadFromFile("robworld.config");

	m_fullscreen = fullscreen;

	CreateRenderer();
	CreateRenderWindow(windowTitle, width, height, fullscreen, parentWindowHandle);

	InitializeResources();

	CreateSceneManager();
	CreateCamera();

	CreateWorld();

	CreateHUD();
	//InitializeUserData();

	InitializeViewStates();
	InitializeInput(captureInput);
}

void WorldView::LoadOgrePlugin(String pluginName)
{
#if _DEBUG
	pluginName += "_d";
#endif
	m_pOgre->loadPlugin(pluginName);
}

void WorldView::CreateRenderer()
{
	m_pOgre = new Ogre::Root("", "");

	// load D3D Ogre plugin
	LoadOgrePlugin("RenderSystem_Direct3D9");

	// for now, use the first render system available
	RenderSystemList renderSystems = m_pOgre->getAvailableRenderers();
	m_pRenderSystem = *(renderSystems.begin());
	m_pOgre->setRenderSystem(m_pRenderSystem);

	//const RenderSystemCapabilities* pCaps = m_pRenderSystem->getCapabilities();
	//debug << "fragment program float constants: " << pCaps->getFragmentProgramConstantFloatCount() << endl;
	
	// always wait for vertical blank if running windowed
	if (!m_fullscreen)
		m_pRenderSystem->setWaitForVerticalBlank(true);

	// load plugins
	LoadOgrePlugin("Plugin_OctreeSceneManager"); 
	LoadOgrePlugin("Plugin_CgProgramManager"); 

	// initialize Ogre without creating the render window
	m_pRenderWindow = m_pOgre->initialise(false);
}

void WorldView::CreateRenderWindow(String windowTitle, 
																	unsigned int width, 
																	unsigned int height, 
																	bool fullscreen, 
																	String parentWindowHandle)
{
	NameValuePairList options;

	// if running in a child window, set externalWindowHandle option to the handle of the parent window
	if (!parentWindowHandle.empty() && !fullscreen)
		options["externalWindowHandle"] = parentWindowHandle;

	m_pRenderWindow = m_pOgre->createRenderWindow(windowTitle, width, height, fullscreen, &options);
	Ogre::WindowEventUtilities::addWindowEventListener(m_pRenderWindow, this);
}

// WindowEventListener methods
void WorldView::windowResized(Ogre::RenderWindow* pRenderWindow)
{
	assert(pRenderWindow == m_pRenderWindow);

	unsigned int width = m_pRenderWindow->getWidth();
	unsigned int height = m_pRenderWindow->getHeight();
	debug << "Render window resized, now " << width << "x" << height << endl;

	if (m_pInput)
		m_pInput->SetWindowExtents(width, height);
}

bool WorldView::windowClosing(Ogre::RenderWindow* pRenderWindow)
{
  m_exitLoop = true;

  // let window close
  return true;
}

void WorldView::windowClosed(Ogre::RenderWindow* pRenderWindow)
{
}

void WorldView::windowFocusChange(Ogre::RenderWindow* pRenderWindow)
{
}



void WorldView::InitializeResources()
{
	ResourceGroupManager& resourceGroupManager = ResourceGroupManager::getSingleton();

	resourceGroupManager.addResourceLocation("Resources", "FileSystem", "General");
	resourceGroupManager.addResourceLocation("Resources/Fonts", "FileSystem", "General");
	resourceGroupManager.addResourceLocation("Resources/Overlays", "FileSystem", "General");
	resourceGroupManager.addResourceLocation("Resources/Materials", "FileSystem", "General");
	resourceGroupManager.addResourceLocation("Resources/Materials/Shaders", "FileSystem", "General");
	resourceGroupManager.addResourceLocation("Resources/Materials/Textures", "FileSystem", "General");
	resourceGroupManager.addResourceLocation("Resources/Materials/Textures/HUD", "FileSystem", "General");
	resourceGroupManager.addResourceLocation("Resources/Materials/Textures/Skies", "FileSystem", "General");
	resourceGroupManager.addResourceLocation("Resources/Materials/Textures/Height Colours", "FileSystem", "General");
	resourceGroupManager.addResourceLocation("Resources/Meshes", "FileSystem", "General");

	resourceGroupManager.addResourceLocation("Data/Terrain/Lightmaps", "FileSystem", "General");

	resourceGroupManager.initialiseAllResourceGroups();
}

void WorldView::CreateSceneManager()
{
	debug << "WorldView: creating scene manager" << endl; 
	m_pSceneManager = m_pOgre->createSceneManager("DefaultSceneManager");

  if (m_config.GetSetting<bool>("EnableShadows"))
    m_pSceneManager->setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE);

  Thing::setSceneManager(m_pSceneManager);
  ThingMaker::init(m_pSceneManager);

	// TODO: initialize the octree properly
	//m_pSceneManager->resize(Ogre::AxisAlignedBox(-1000000, -1000000, -1000000, 1000000, 1000000, 1000000));
}

void WorldView::CreateCamera()
{
	m_pCamera = m_pSceneManager->createCamera("camera");
	
	// TODO proper camera parameters
	m_pCamera->setNearClipDistance(m_config.GetSetting<float>("NearClipDistance", 10.0f));
	m_pCamera->setFarClipDistance(m_config.GetSetting<float>("FarClipDistance", 100000.0f));

//	Vector3 cameraPosition = World::UP * 1.8;
//	Vector3 cameraPosition(-5500.0, -4000.0, 1.8);
//	m_pCamera->setPosition(cameraPosition);

	m_pCamera->setFixedYawAxis(true, World::UP);
//  m_pCamera->lookAt(cameraPosition + World::EAST);
	
  //m_pCamera->setPolygonMode(Ogre::PolygonMode::PM_WIREFRAME);

	// create viewport with this camera attached
  m_pViewport = m_pRenderWindow->addViewport(m_pCamera);
	m_pCamera->setAspectRatio(Real(m_pViewport->getActualWidth()) / Real(m_pViewport->getActualHeight()));


	// set background to blue
  m_pViewport->setBackgroundColour(Ogre::ColourValue(0,0,0.25));

	//m_pCamera->setDebugDisplayEnabled(true);
	//m_pCamera->setVisible(true);
	//m_pSceneManager->getRootSceneNode()->attachObject(m_pCamera);


}

void WorldView::CreateHUD()
{
	m_pHUD = new HeadUpDisplay(m_config, m_pWorld);
}

void WorldView::InitializeInput(bool captureInput)
{
	// initialize input
	HWND hWnd;
	m_pRenderWindow->getCustomAttribute("WINDOW", &hWnd);
	m_pInput = new Input(hWnd, captureInput, m_fullscreen, m_commandQueue);
	m_pInput->SetWindowExtents(m_pRenderWindow->getWidth(), m_pRenderWindow->getHeight());

	m_pInput->SetThingUnderControl(m_pWorld->GetFirstPerson());
}

InputState& WorldView::GetInputState()
{ 
	return m_pInput->GetInputState(); 
}

void WorldView::CreateWorld()
{
	m_pWorld = new World(m_config, this, m_pSceneManager, m_pSceneManager->getRootSceneNode());
  m_pWorld->createContents();
	EnableFog(true);
}

void WorldView::InitializeViewStates()
{
	WorldViewState s;
	
	// default, no HUD
	s.name = "default, no HUD";
	s.showHUD = false;
	AddViewState(s);

	// default, with HUD
	s.name = "default, with HUD";
	s.showHUD = true;
	AddViewState(s);

	// default, no HUD, wireframe
	s.name = "default, no HUD, wireframe";
	s.wireframe = true;
	AddViewState(s);

	// set first view state as default
	SetNextViewState();
}

void WorldView::AddViewState(const WorldViewState& s)
{
	m_viewStates[s.name] = s;
}

void WorldView::SetNextViewState()
{
	if (m_pViewState == NULL)
		m_pViewState = &m_viewStates.begin()->second;
	else
	{
		std::map<String, WorldViewState>::iterator it = m_viewStates.find(m_pViewState->name);
		
    if (++it == m_viewStates.end())
			it = m_viewStates.begin();

		SetViewState(&it->second);
	}
}

void WorldView::SetViewState(String name)
{
	SetViewState(&m_viewStates.find(name)->second);
}

void WorldView::SetViewState(WorldViewState* pViewState)
{
	m_pViewState = pViewState;
	debug << "WorldView view state: " << m_pViewState->name << endl;

  if (m_pViewState->wireframe)
    m_pViewport->getCamera()->setPolygonMode(Ogre::PolygonMode::PM_WIREFRAME);
  else
    m_pViewport->getCamera()->setPolygonMode(Ogre::PolygonMode::PM_SOLID);
}

void WorldView::ToggleMetaCam()
{
  if (m_pViewport->getCamera() == m_pCamera)
  {
    if (m_pMetaCam == NULL)
    {
      m_pMetaCam = m_pSceneManager->createCamera("MetaCam");
      m_pMetaCam->setFixedYawAxis(true, World::UP);

      m_pDummyCam = m_pSceneManager->createCamera("DummyCam");
      m_pDummyCam->setDebugDisplayEnabled(true);
      SceneNode* p = m_pSceneManager->createSceneNode("camera");
      p->attachObject(m_pDummyCam);
      m_pSceneManager->getRootSceneNode()->addChild(p);
    }

    m_pMetaCam->synchroniseBaseSettingsWith(m_pCamera);

    m_pDummyCam ->synchroniseBaseSettingsWith(m_pCamera);
    m_pDummyCam->setVisible(true);

    SceneNode* p = m_pDummyCam->getParentSceneNode(); 
    p->setPosition(m_pDummyCam->getPosition());
    p->setOrientation(m_pDummyCam->getOrientation());
    m_pDummyCam->setPosition(Vector3::ZERO);
    m_pDummyCam->setOrientation(Quaternion::IDENTITY);

    m_pViewport->setCamera(m_pMetaCam);

    m_pWorld->FirstPerson->Camera = m_pMetaCam;

    debug << "Switched to meta camera" << endl;
  }
  else
  {
    m_pDummyCam->setVisible(false);

    m_pCamera->synchroniseBaseSettingsWith(m_pMetaCam);

    m_pViewport->setCamera(m_pCamera);
    m_pWorld->FirstPerson->Camera = m_pCamera;

    debug << "Switched to proper camera" << endl;
  }
}

void WorldView::loop()
{
  Clock clock;
  Real lastTime = 0.0;
  while (!m_exitLoop)
  {
    Real time = clock.seconds();
    Real deltaTime = time - lastTime;
    lastTime = time;

    Update(deltaTime);

    WindowEventUtilities::messagePump();
    m_pOgre->renderOneFrame();
  }
}

/*
void WorldView::StartRenderLoop()
{
	debug << "WorldView: starting render loop" << endl;
	m_pOgre->addFrameListener(this);
	m_exitLoop = false;
	m_frameNumber = 0;
	m_pOgre->startRendering();
}
*/
void WorldView::StopRenderLoop()
{
	debug << "WorldView: stopping render loop" << endl;
	m_exitLoop = true;
}

void WorldView::RenderOneFrame()
{
	//if (m_pOgre)
	//	m_pOgre->renderOneFrame();
}

/*
bool WorldView::frameStarted(const FrameEvent& frame)
{
	if (m_exitLoop)
	{
		return false;
	}
	else if (m_pRenderWindow->isClosed())
	{
		// inform listener
		OnRenderWindowClosed();

		// stop rendering
		return false;
	}
	else
	{
		OnFrameStarted();

		Update(frame.timeSinceLastFrame);
		return true;
	}
}

bool WorldView::frameEnded(const FrameEvent& frame)
{
	OnFrameEnded();

	m_frameNumber++;
	return !m_exitLoop;
}
*/

void WorldView::Update(Real deltaTime)
{
	if (m_pInput != NULL)
		m_pInput->ProcessInput();

	ProcessCommandQueue();

	m_pWorld->Update(deltaTime);

	UpdateHUD();
}

void WorldView::ProcessCommandQueue()
{
	while(!m_commandQueue.empty())
	{
		Command* pCommand = m_commandQueue.front();
		m_commandQueue.pop();

		if (pCommand->IsSystemCommand())
		{
			pCommand->Execute();
			delete pCommand;
		}
		else
		{
			m_pWorld->GetCommandQueue().push(pCommand);
		}
	}
}

void WorldView::UpdateHUD()
{
	// HUD visible in current view state?
	if (!m_pViewState->showHUD)
	{
		if (m_pHUD->Visible)
			m_pHUD->Visible = false;
		return;
	}
	if (!m_pHUD->Visible)
		m_pHUD->Visible = true;


	// TODO: move all this into specialized classes
	std::ostringstream o;
	o << "fps: " << setprecision(4) << m_pRenderWindow->getLastFPS();
	o << ", triangles: " << m_pRenderWindow->getTriangleCount();
	o << ", batches: " << m_pRenderWindow->getBatchCount();
	//o << "mouse pos: " << m_pInput->GetInputState().MouseNormalizedX << ", "  << m_pInput->GetInputState().MouseNormalizedY; 

	o << endl;
	o << m_pWorld->GetDebugText();
	//o << m_pInput->GetDebugText();
	m_pHUD->SetDebugInfoText(o.str());




	m_pHUD->Update();
}

void WorldView::OnRenderWindowClosed()
{
	if (m_pListener)
		m_pListener->RenderWindowClosed();
}

void WorldView::OnFrameStarted()
{
	if (m_pListener)
		m_pListener->FrameStarted();
}

void WorldView::OnFrameEnded()
{
	if (m_pListener)
		m_pListener->FrameEnded();
}

//const GeoLocation& WorldView::GetFirstPersonLocation()
//{
//	return m_pWorld->GetFirstPerson()->GetLocation();
//}

void WorldView::WorldResized(const Ogre::AxisAlignedBox& boundingBox)
{
	debug << "WorldView: world resized, updating scene manager octree size to match" << endl;
	//m_pSceneManager->resize(boundingBox);
}

void WorldView::ToggleWireFrame()
{
	if (!m_pCamera)
		return;

	bool enableWireFrame = m_pCamera->getPolygonMode() != Ogre::PolygonMode::PM_WIREFRAME;

	if (enableWireFrame)
	{
		m_pCamera->setPolygonMode(Ogre::PolygonMode::PM_WIREFRAME);
		EnableFog(false);
		m_pCamera->getViewport()->setBackgroundColour(Ogre::ColourValue(0,0,0.25));
	}
	else
	{
		m_pCamera->setPolygonMode(Ogre::PolygonMode::PM_SOLID);
		EnableFog(true);
	}
}

void WorldView::EnableFog(bool enable)
{
	if (enable)
	{
		//// set background colour to fog colour even if fogging is disabled, it looks nicer
		//Ogre::Viewport* pViewport = m_pCamera->getViewport();
		//pViewport->setBackgroundColour(m_pWorld->GetFogColor());

		// allow fog to be switched off in config
		if (m_config.GetSetting<bool>("DisableFog", false))
		{
			debug << "Fog disabled in config file" << endl;
			m_pSceneManager->setFog(Ogre::FOG_NONE);
		}
		else
		{
			m_pSceneManager->setFog(Ogre::FOG_EXP, m_pWorld->FogColor, m_pWorld->FogExponent);
		}
	}
	else
	{
		m_pSceneManager->setFog(Ogre::FOG_NONE);
	}
}

/*
void WorldView::InitializeUserData()
{
	m_pUserData = new UserData::UserDataManager(m_pSceneManager->getRootSceneNode()->createChildSceneNode("User Data"));
	
	// TODO: load multiple .kml files

	if (m_config.HasSetting("UserDataPath"))
	{
		string userDataPath = m_config.GetSetting<string>("UserDataPath");

		m_pUserData->LoadDataSet(userDataPath);

		m_pUserData->CreateAllWorldThings(m_pWorld);
	}
}
*/

void WorldView::ThingAddedToWorld(Thing* pThing)
{
	if (pThing->ShowLabel)
		m_pHUD->AddThingLabel(pThing, pThing->Name);
}

void WorldView::TakeScreenshot()
{
	m_pRenderWindow->writeContentsToTimestampedFile("Screenshot_", ".png");
}

SceneNode* WorldView::createSceneNode(const String& name)
{
  return s_pInstance->m_pSceneManager->createSceneNode(name);
}