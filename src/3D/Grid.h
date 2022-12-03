#pragma once

#include "Geometry.h"

class Grid : public Geometry
{
protected:
  int m_widthVerts, m_heightVerts;
  float m_width, m_height;

public:
  Grid(int widthVerts, int heightVerts, 
       Ogre::ManualObject* pManualObject, 
       const Ogre::String& materialName, 
       float width = 1.0, float height = 1.0);
  virtual ~Grid(){}

  virtual void createGeometry();

  void triangulate();

};
