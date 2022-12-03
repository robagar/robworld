#include "../../StdAfx.h"

#include "Perturbation2DSineX.h"

using namespace Ogre;

Perturbation2DSineX::Perturbation2DSineX(Real amplitude, Real wavelength, Real speed)
  : Perturbation2D(-amplitude, amplitude),
    m_amplitude(amplitude),
    m_wavelength(wavelength),
    m_speed(speed)
{
}

Ogre::Real Perturbation2DSineX::valueAt(Ogre::Real x, Ogre::Real y, Ogre::Real time, Ogre::Real deltaTime)
{
  return m_amplitude * (Math::Sin(Math::TWO_PI * ((x + time * m_speed) / m_wavelength)));
}
 