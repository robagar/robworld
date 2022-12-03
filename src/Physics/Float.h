#pragma once

class Ocean;

class Float
{
  String m_name;
  Real m_length;
  Real m_volume;
  Vector3 m_localPosition;

  Vector3 m_worldPosition;
  Vector3 m_force;

public:
  Float(const String& name, Real length, Real volume, Vector3 localPosition = Vector3::ZERO);

  void applyBouyancyForce(Ocean* pOcean, RigidThing* pThing);

  String getDebugText();

};