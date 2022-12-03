#include "../StdAfx.h"
#include "Grid.h"

using namespace Ogre;

Grid::Grid(int widthVerts, int heightVerts, 
           ManualObject* pManualObject, 
           const String& materialName, 
           float width, float height)
  : Geometry(pManualObject, materialName), 
    m_widthVerts(widthVerts),
    m_heightVerts(heightVerts),
    m_width(width),
    m_height(height)
{
  m_pManualObject->setCastShadows(false);
  createGeometry();
}

void Grid::createGeometry()
{
  m_pManualObject->setDynamic(true);
  m_pManualObject->begin(m_materialName);

  // vertices
  float deltaX = m_width / (m_widthVerts - 1);
  float deltaY = m_height / (m_heightVerts - 1);

  float x0 = -m_width / 2, x = x0, y = -m_height / 2;
  for(int j=0; j<m_heightVerts; ++j)
  {
    for(int i=0; i<m_widthVerts; ++i)
    {
      m_pManualObject->position(x, y, 0);
      m_pManualObject->normal(Vector3::UNIT_Z);
      x += deltaX;
    }
    
    x = x0;
    y += deltaY;
  }

  // indices
  triangulate();

  m_pManualObject->end();
}

void Grid::triangulate()
{
	/*
		Triangle list:

		|    \|    \|    \|
		+-----+-----+-----+--
		|\  1 |\  3 |\  5 |\
		|  \  |  \  |  \  |
		| 0  \| 2  \| 4  \|
		+-----+-----+-----+--
	*/
  int a = 0;
  for(int j=0; j<m_heightVerts-1; ++j)
  {
    for(int i=0; i<m_widthVerts-1; ++i)
    {
      /*
       c       d
    		+-----+
    		|\  1 |
    		|  \  |
    		| 0  \|
     		+-----+
       a       b
	    */

      int b = a + 1;
      int c = a + m_widthVerts;
      int d = c + 1;

      m_pManualObject->triangle(a, b, c);
      m_pManualObject->triangle(c, b, d);

      //m_pManualObject->quad(a, b, d, c);

      a++;
    }
    a++;
  }
}

