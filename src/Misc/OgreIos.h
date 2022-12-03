#pragma once

#include <iostream>
#include <Ogre.h>

/**
 * @brief 
 * Extraction operator for x,y coordinate pairs.
 * 
 * @param is
 * @param v
 * @return the input stream
 */
inline std::istream& operator >> (std::istream& is, Ogre::Vector2& v)
{
	double x, y;
	char comma;
	is >> std::ws >> x;
	is >> std::ws >> comma;
	is >> std::ws >> y;
	v.x = (Ogre::Real)x;
	v.y = (Ogre::Real)y;
	return is;
}

inline std::istream& operator >> (std::istream& is, Ogre::ColourValue& c)
{
	char comma;
	is >> std::ws >> c.r;
	is >> std::ws >> comma;
	is >> std::ws >> c.g;
	is >> std::ws >> comma;
	is >> std::ws >> c.b;
	is >> std::ws >> comma;
	is >> std::ws >> c.a;
	return is;
}