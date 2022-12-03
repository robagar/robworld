#pragma once


// Ogre -> Bullet conversions
btScalar ogre2bullet(const Ogre::Real& r);
btVector3 ogre2bullet(const Ogre::Vector3& v);
btQuaternion ogre2bullet(const Ogre::Quaternion& q);

// Bullet -> Ogre conversions
Ogre::Real bullet2ogre(const btScalar& s);
Ogre::Vector3 bullet2ogre(const btVector3& v);
Ogre::Quaternion bullet2ogre(const btQuaternion& q);