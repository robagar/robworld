#pragma once

#include "Perturbation2D.h"

class PerturbedHorizontal
{
  Ogre::Plane m_base;
  Ogre::Plane m_lower;
  Ogre::Plane m_upper;

  std::vector<Perturbation2D*> m_perturbations;

public:
  PerturbedHorizontal(const Ogre::Real& = 0);

  const Ogre::Plane& getBase() const { return m_base; }
  __declspec(property(get=getBase)) Ogre::Plane& Base;

  const Ogre::Plane& getLower() const { return m_lower; }
  __declspec(property(get=getLower)) Ogre::Plane& Lower;

  const Ogre::Plane& getUpper() const { return m_upper; }
  __declspec(property(get=getUpper)) Ogre::Plane& Upper;

  Ogre::Real getBaseHeight() const { return -m_base.d; }
  __declspec(property(get=getBaseHeight)) Ogre::Real BaseHeight;

  void addPerturbation(Perturbation2D* pPerturbation);

  void perturb(Ogre::Vector3&, Ogre::Real time, Ogre::Real deltaTime);

  bool contains(const Ogre::Vector3&) const;

  Ogre::Real levelAt(Ogre::Real x, Ogre::Real y, Ogre::Real time, Ogre::Real deltaTime);

private:
  void updateBounds();
};