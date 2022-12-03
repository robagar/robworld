#pragma once

#include "SubdividedTriangle.h"

class Sail : public SoftThing
{
  class Boat* m_pBoat;
  class Boom* m_pBoom;

  SubdividedTriangle* m_pGeometry;

public:
  Sail(const string& name, const Ogre::Vector3& head, const Ogre::Vector3& tack, const Ogre::Vector3& clew);
  virtual ~Sail();

  virtual Ogre::Vector3 getWorldVelocity() const;

  void attachToBoat(Boat*, Boom*);

  const std::vector<Ogre::uint32> luffEdgeIndices() const;
  const std::vector<Ogre::uint32> footEdgeIndices() const;
  const std::vector<Ogre::uint32> leachEdgeIndices() const;

  void updateSoftBodyGeometry();

  Ogre::Vector3 getNormal() const;
  __declspec(property(get=getNormal)) Ogre::Vector3 Normal;

  void blow(const Ogre::Vector3& windVelocity);
  virtual void updatePhysics(Ogre::Real deltaTime);

  Ogre::Real projectedArea(const Ogre::Vector3& planeNormal);

};