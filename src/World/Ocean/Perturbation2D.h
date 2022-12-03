#pragma once


class Perturbation2D
{
  Ogre::Real m_min, m_max;

protected:
  Perturbation2D(Ogre::Real amplitude) : m_min(-amplitude), m_max(amplitude) {}
  Perturbation2D(Ogre::Real min, Ogre::Real max) : m_min(min), m_max(max) {} 

public:
  virtual ~Perturbation2D() {}
  virtual Ogre::Real valueAt(Ogre::Real x, Ogre::Real y, Ogre::Real time, Ogre::Real deltaTime) PURE;

  Ogre::Real getMin() { return m_min; }
  __declspec(property(get=getMin)) Ogre::Real Min;

  Ogre::Real getMax() { return m_max; }
  __declspec(property(get=getMax)) Ogre::Real Max;


};