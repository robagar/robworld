#pragma once

#include "Perturbation2D.h"

class Perturbation2DSineX : public Perturbation2D
{
  Ogre::Real m_amplitude;
  Ogre::Real m_wavelength;
  Ogre::Real m_speed;

public: 
  Perturbation2DSineX(Ogre::Real amplitude, Ogre::Real wavelength, Ogre::Real speed);

  Ogre::Real valueAt(Ogre::Real x, Ogre::Real y, Ogre::Real time, Ogre::Real deltaTime);
};