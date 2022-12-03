#include "../../StdAfx.h"

#include "Ripples.h"

Ripples::Ripples()
  : Perturbation2DPerlinNoise(0.03f, 0.5f, 1.0f)
{
}

Real Ripples::valueAt(Real x, Real y, Real t, Real dt)
{
  Real v = Perturbation2DPerlinNoise::valueAt(x, y, t, dt);
  return v;
}