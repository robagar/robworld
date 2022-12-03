#pragma once

class Geometry
{
protected:
  struct Vertex
  {
    Ogre::Vector3 position;
    Ogre::Vector3 normal;
    Ogre::uint32 flags;
  };

  Ogre::ManualObject* m_pManualObject;
  Ogre::String m_materialName;

  std::vector<Geometry::Vertex> m_vertices;
  std::vector<Ogre::uint32> m_indices;

  int m_numTriangles;

  bool m_doubleSided;

public:
  Geometry(Ogre::ManualObject* pManualObject,  const Ogre::String& materialName);

  Ogre::ManualObject* getManualObjectPtr() { return m_pManualObject; }
  __declspec(property(get=getManualObjectPtr)) Ogre::ManualObject* ManualObjectPtr;

  const Ogre::Vector3 getVertexPosition(Ogre::uint32 index) const;

  const std::vector<Ogre::Vector3> getVertexPositions() const;
  __declspec(property(get=getVertexPositions)) const std::vector<Ogre::Vector3> VertexPositions;
  /*
  const std::vector<Ogre::Vector3>& getNormals() const { return m_normals; }
  __declspec(property(get=getNormals)) const std::vector<Ogre::Vector3>& Normals;
  */
  const std::vector<Ogre::uint32>& getIndices() const { return m_indices; }
  __declspec(property(get=getIndices)) const std::vector<Ogre::uint32>& Indices;

  const int getNumTriangles() const { return m_numTriangles; }
  __declspec(property(get=getNumTriangles)) const int NumTriangles; 

  const bool getDoubleSided() const { return m_doubleSided; }
  void setDoubleSided(bool _doubleSided) { m_doubleSided = _doubleSided; }
  __declspec(property(get=getDoubleSided, put=setDoubleSided)) bool DoubleSided;

  virtual void createGeometry() PURE;
  virtual void updateGeometry();

  void setVertex(int index, const Ogre::Vector3& position, const Ogre::Vector3& normal);


protected:
  Ogre::uint32 addVertex(const Ogre::Vector3& position, const Ogre::Vector3& normal, const Ogre::uint32 flags = 0);
  Ogre::uint32 addVertexIfNew(const Ogre::Vector3& position, const Ogre::Vector3& normal, const Ogre::uint32 flags = 0);
  void addTriangle(const Ogre::uint32 i, const Ogre::uint32 j, const Ogre::uint32 k);
};