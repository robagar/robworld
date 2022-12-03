#pragma once

#include "TerrainTileParams.h"

class World;

namespace NonUniformTerrain
{
	class Terrain;
	class TerrainPage;
	class TerrainTileIndexDataManager;

	class TerrainTile : public Ogre::Renderable, public Ogre::MovableObject
	{
		Terrain* m_pTerrain;
		TerrainPage* m_pPage;
		Ogre::SceneNode* m_pSceneNode;

		TerrainTileParams m_params;
		TerrainTileIndexDataManager* m_pTerrainTileIndexDataManager;

		TerrainTile* m_neighbours[4];

		static String s_movableType;

		// number of vertices per side
		short m_size;

		// world space coordinates of edges
		float m_west;
		float m_south;
		float m_width;

		// vertex coordinates
		short m_qNorth;
		short m_pEast;
		short m_qSouth;
		short m_pWest;

		Ogre::Vector3 m_center;
		Ogre::Real m_boundingRadius;
		Ogre::AxisAlignedBox m_boundingBox;
		mutable Ogre::LightList m_lights;

		short m_oldLOD;
		short m_highestAvailableLOD;
		short m_lowestAvailableLOD;

		std::vector<float> m_lodMaxDistances;

		Ogre::IndexData* m_pIndexData;

	public:
		TerrainTile(TerrainPage* pPage,
								Ogre::SceneNode* pSceneNode,	
								short i, short j, 
								short size, 
								float width,
								TerrainTileIndexDataManager* pTerrainTileIndexDataManager);
		~TerrainTile();

		const TerrainTileParams& GetParams() { return m_params; }

		// Ogre::Renderable overrides
		virtual const Ogre::MaterialPtr& getMaterial() const;
		virtual void getRenderOperation(Ogre::RenderOperation& op);
		virtual void getWorldTransforms(Ogre::Matrix4* xform) const;
		virtual const Ogre::Quaternion& getWorldOrientation() const;
		virtual const Ogre::Vector3& getWorldPosition() const;
		virtual Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const;
		virtual const Ogre::LightList& getLights() const;

		// Ogre::MovableObject overrides
		virtual const Ogre::String& getMovableType() const;
		virtual void _updateRenderQueue(Ogre::RenderQueue* pQueue);
		virtual void _notifyCurrentCamera(Ogre::Camera* pCamera);
		virtual const Ogre::AxisAlignedBox& getBoundingBox() const { return m_boundingBox; }
		virtual Ogre::Real getBoundingRadius() const { return m_boundingRadius; } 
		virtual void visitRenderables(Ogre::Renderable::Visitor *,bool) {}

		void SetNeighbour(Neighbour neighbour, TerrainTile* pTile) { m_neighbours[neighbour] = pTile; }
		TerrainTile* GetNeighbour(Neighbour neighbour) { return m_neighbours[neighbour]; }

	private:
		void CalculateBoundingBox();

		void CalculateLODMaxDistances();
		short GetLODForDistance(float d) const;

		void UpdateLOD(const Ogre::Vector3& vCameraPosition);
		bool UpdateNeighbourLODs();

		void UpdateIndexData();

	};
}
