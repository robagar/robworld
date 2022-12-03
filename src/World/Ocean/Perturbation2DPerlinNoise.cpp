#include "../../StdAfx.h"
#include "Perturbation2DPerlinNoise.h"

#include <noise.h>

using namespace Ogre;

Perturbation2DPerlinNoise::Perturbation2DPerlinNoise(Real amplitude, Real scaleSize, Real scaleTime)
  : Perturbation2D(amplitude),
    m_amplitude(amplitude),
    m_scaleSize(scaleSize),
    m_scaleTime(scaleTime),
    m_pPerlin(new noise::module::Perlin())
{
}

Perturbation2DPerlinNoise::~Perturbation2DPerlinNoise()
{
  delete m_pPerlin;
}

Real Perturbation2DPerlinNoise::valueAt(Real x, Real y, Real time, Real deltaTime)
{
  return m_amplitude * (Real)m_pPerlin->GetValue(x / m_scaleSize, y / m_scaleSize, time / m_scaleTime);
}

