#include "../../StdAfx.h"

#include "PerturbedHorizontal.h"

using namespace Ogre;

PerturbedHorizontal::PerturbedHorizontal(const Real& d)
  : m_base(Vector3::UNIT_Z, d),
    m_lower(m_base),
    m_upper(m_base)

{
}

void PerturbedHorizontal::addPerturbation(Perturbation2D* pPerturbation) 
{ 
  m_perturbations.push_back(pPerturbation); 
  updateBounds(); 
}

void PerturbedHorizontal::updateBounds()
{
  Real lower = 0, upper = 0;

  foreach(Perturbation2D*& p, m_perturbations)
  {
     lower = min(lower, p->Min);
     upper = max(upper, p->Max);
  }

  m_lower.d = m_base.d - lower;
  m_upper.d = m_base.d - upper;
}

void PerturbedHorizontal::perturb(Vector3& point, Ogre::Real time, Ogre::Real deltaTime)
{
  foreach(Perturbation2D*& p, m_perturbations)
    point.z += p->valueAt(point.x, point.y, time, deltaTime);
}

bool PerturbedHorizontal::contains(const Ogre::Vector3& point) const
{
  return point.z >= m_lower.d && point.z <= m_upper.d;
}

Real PerturbedHorizontal::levelAt(Ogre::Real x, Ogre::Real y, Ogre::Real time, Ogre::Real deltaTime)
{
  Vector3 p(x, y, BaseHeight);
  perturb(p, time, deltaTime);
  return p.z;
}