#pragma once

#include "Perturbation2D.h"

namespace noise
{
  namespace module
  {
    class Perlin;
  }
}

class Perturbation2DPerlinNoise : public Perturbation2D
{
  Ogre::Real m_amplitude, m_scaleSize, m_scaleTime;

  noise::module::Perlin* m_pPerlin;

public: 
  Perturbation2DPerlinNoise(Ogre::Real amplitude, Ogre::Real scaleSize, Ogre::Real scaleTime);
  virtual ~Perturbation2DPerlinNoise();

  virtual Ogre::Real valueAt(Ogre::Real x, Ogre::Real y, Ogre::Real time, Ogre::Real deltaTime);

};