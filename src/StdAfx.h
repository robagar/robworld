#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// boost
#define BOOST_FILESYSTEM_DYN_LINK
#define BOOST_FILESYSTEM_VERSION 2
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#include <boost/algorithm/string.hpp>

#define BOOST_THREAD_DYN_DLL
#include <boost/thread/thread.hpp>

// Ogre
#include <Ogre.h>
#include <OgrePanelOverlayElement.h>
#include <OgreTextAreaOverlayElement.h>
using namespace Ogre;

// Bullet Physics
#include "btBulletDynamicsCommon.h"
#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"
#include "BulletSoftBody/btSoftBodyHelpers.h"

// Debug  
#include "Debug/Debug.h"

// Misc
#include "Misc/MiscNumeric.h"
#include "Misc/Misc.h"
#include "Misc/Config.h"
#include "Misc/OgreIos.h"

// Geographical
#include "Geo/GeoLocation.h"

// allow STL to be used without std::
//using namespace std;
using std::endl;
using std::min;
using std::max;
using std::setprecision;
using std::setw;