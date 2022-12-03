#pragma once

#include "Perturbation2DPerlinNoise.h"

class Ripples : public Perturbation2DPerlinNoise
{
public:

  Ripples();

  virtual Ogre::Real valueAt(Ogre::Real x, Ogre::Real y, Ogre::Real time, Ogre::Real deltaTime);

};