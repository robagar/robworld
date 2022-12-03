#pragma once

// misc
//using namespace std;

#define PURE =0
//#define NULL (0)

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }

#define FLOAT_MAX 3.402823466e38f
#define FLOAT_MIN 3.4e-38f

#define LOG(s) {ostringstream logStream; logStream << s; Ogre::LogManager::getSingleton().logMessage(logStream.str());} 

template<class T, class V> bool contains(const T& collection, const V& value)
{
	return collection.find(value) != collection.end();
}

template<class T> T clamp (const T& val, const T& min, const T& max)
{
	if (val < min) return min;
	if (val > max) return max;
	return val;
}

template<class T> T lerp (const T& a, const T& b, const Ogre::Real t)
{
  return a + (b - a) * t;
}

