#pragma once

//#include "../../Physics/ThingBulletMotionState.h"

class World;
class Float;

class Thing : public Ogre::Any
{
public:
  static const Ogre::Vector3 FORWARDS;
  static const Ogre::Vector3 BACKWARDS;
  static const Ogre::Vector3 RIGHT;
  static const Ogre::Vector3 LEFT;
  static const Ogre::Vector3 UP;
  static const Ogre::Vector3 DOWN;

protected:
  static Ogre::SceneManager* s_pSceneManager;

	World* m_pWorld;
	String m_name;
	
	float m_boundingSphereRadius;

  Thing* m_pParent;
  std::vector<Thing*> m_children;


	Geo::GeoLocation m_location;

  // Ogre graphics properties
  Ogre::SceneNode* m_pSceneNode;
	Ogre::MovableObject* m_pRendererObject;

  std::vector<Float*> m_floats;

	bool m_showLabel;

public:
  static void setSceneManager(Ogre::SceneManager* pSceneManager) { s_pSceneManager = pSceneManager; }
protected:
  static Ogre::SceneNode* createSceneNode(const String& name) { return s_pSceneManager->createSceneNode(name); }

public:
	Thing(const String& name);
	Thing(const String& name, const String& meshName, const String& materialName = "default");
	virtual ~Thing();

	void SetWorld(World* pWorld) { m_pWorld = pWorld; }
	__declspec(property(put=SetWorld)) World* World;

	const std::string& GetName() const { return m_name; }
	__declspec(property(get=GetName)) const std::string& Name;


  // position in world space
	virtual const Vector3& getWorldPosition() const;
	virtual void setWorldPosition(const Vector3& position); 
	__declspec(property(get=getWorldPosition, put=setWorldPosition)) Vector3& WorldPosition;

  // position relative to parent in parent local coordinates
	virtual const Vector3& getLocalPosition() const;
	virtual void setLocalPosition(const Vector3& position); 
	__declspec(property(get=getLocalPosition, put=setLocalPosition)) Vector3& LocalPosition;

  // orientation in world space
  virtual const Quaternion& getWorldOrientation() const;
	virtual void setWorldOrientation(const Quaternion& orientation);
	__declspec(property(get=getWorldOrientation, put=setWorldOrientation)) Quaternion& WorldOrientation;

  // orientation relative to parent
  virtual const Quaternion& getLocalOrientation() const;
	virtual void setLocalOrientation(const Quaternion& orientation);
	__declspec(property(get=getLocalOrientation, put=setLocalOrientation)) Quaternion& LocalOrientation;

  // local scaling
  virtual Real getScale() const;
  virtual void setScale(Real scale);
  __declspec(property(get=getScale, put=setScale)) Real Scale;

	void UpdateLocation(); 
	const Geo::GeoLocation& GetLocation() const { return m_location; }
	void SetLocation(const Geo::GeoLocation& l);
	__declspec(property(get=GetLocation, put=SetLocation)) Geo::GeoLocation& Location;

	virtual Vector3 getDirection() const;
  virtual void setDirection(const Vector3&);
	__declspec(property(get=getDirection, put=setDirection)) Vector3 Direction;

	virtual Vector3 getUpDirection() const;
  virtual void setUpDirection(const Vector3&);
	__declspec(property(get=getUpDirection, put=setUpDirection)) Vector3 UpDirection;


	Ogre::Real getHeading() const;
  void setHeading(const Real&);  
	__declspec(property(get=getHeading, put=setHeading)) Ogre::Real Heading; 




  // Ogre renderable
	Ogre::MovableObject* getRendererObject() const { return m_pRendererObject; }
	void setRendererObject(Ogre::MovableObject* pRendererObject);
	__declspec(property(get=getRendererObject, put=setRendererObject)) Ogre::MovableObject* RendererObject;

  // Ogre scene node
  Ogre::SceneNode* getSceneNode() { return m_pSceneNode; }
  __declspec(property(get=getSceneNode)) Ogre::SceneNode* SceneNode;



  // child things
  std::vector<Thing*>& getChildren() { return m_children; }
  __declspec(property (get=getChildren)) std::vector<Thing*>& Children;

  Thing* createChild(const String& name, 
                     const String& meshName, 
                     const Ogre::Vector3& position = Ogre::Vector3::ZERO, 
                     const Ogre::Quaternion& orientation = Ogre::Quaternion::IDENTITY);
  void attachChild(Thing* pChild); 
  bool isChild();

	float GetBoundingSphereRadius() { return m_boundingSphereRadius; }

	bool GetShowLabel(){ return m_showLabel; }
	void SetShowLabel(bool showLabel) { m_showLabel = showLabel; }
	__declspec(property(get=GetShowLabel, put=SetShowLabel)) bool ShowLabel;

	virtual bool IsMovable() { return false; }

  virtual std::string getDebugText();

  // physics
  virtual void setPhysicsBody(btCollisionObject* pPhysicsBody) {}
  virtual btCollisionObject* getPhysicsBody() { return NULL; }
  __declspec(property(get=getPhysicsBody, put=setPhysicsBody)) btCollisionObject* PhysicsBody;

  virtual void setWorldVelocity(const Ogre::Vector3&) {} 
  virtual Ogre::Vector3 getWorldVelocity() const { return Ogre::Vector3::ZERO; };
  __declspec(property(get=getWorldVelocity, put=setWorldVelocity)) Ogre::Vector3 WorldVelocity;

  Ogre::Real getSpeed() const { return WorldVelocity.length(); }
  __declspec(property(get=getSpeed)) Ogre::Real Speed;

  virtual void setLocalVelocity(const Ogre::Vector3&) {} 
  virtual Ogre::Vector3 getLocalVelocity() const { return Ogre::Vector3::ZERO; };
  __declspec(property(get=getLocalVelocity, put=setLocalVelocity)) Ogre::Vector3 LocalVelocity;

  virtual void updatePhysics(Ogre::Real deltaTime){}

  // floats
  void addFloat(Float* pFloat) { m_floats.push_back(pFloat); }
  std::vector<Float*>& getFloats() { return m_floats; }
  __declspec(property(get=getFloats)) std::vector<Float*>& Floats;

  bool isBouyant() { return !m_floats.empty(); }

  // actions
  void throwThing(Thing*, Real speed = 0);
  virtual void blow(const Ogre::Vector3& windVelocity){}

};

typedef std::vector<Thing*> ThingVec;
typedef ThingVec::iterator ThingVecIt;
