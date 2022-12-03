#include "../StdAfx.h"
#include "BulletUtil.h"

// Ogre -> Bullet conversions

btScalar ogre2bullet(const Ogre::Real& r)
{
  return btScalar(r);
}

btVector3 ogre2bullet(const Ogre::Vector3& v)
{
  return btVector3(v.x, v.y, v.z);
}

btQuaternion ogre2bullet(const Ogre::Quaternion& q)
{
  return btQuaternion(q.x, q.y, q.z, q.w);
}


// Bullet -> Ogre conversions

Ogre::Real bullet2ogre(const btScalar& s)
{
  return Ogre::Real(s);
}

Ogre::Vector3 bullet2ogre(const btVector3& v)
{
  return Ogre::Vector3(v.x(), v.y(), v.z());
}

Ogre::Quaternion bullet2ogre(const btQuaternion& q)
{
  return Ogre::Quaternion(q.w(), q.x(), q.y(), q.z());
}