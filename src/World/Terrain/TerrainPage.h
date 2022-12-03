#pragma once

#include "BulletCollision/CollisionShapes/btHeightFieldTerrainShape.h"

#include "TerrainParams.h"
#include "TerrainPageData.h"
#include "TerrainTileNeighbour.h"

namespace NonUniformTerrain
{
	class Terrain;
	class TerrainTile;
	class TerrainTileIndexDataManager;

	class TerrainPage
	{
		Terrain* m_pTerrain;

		const TerrainParams& m_terrainParams;
		Ogre::SceneNode* m_pSceneNode;
		TerrainTileIndexDataManager* m_pTerrainTileIndexDataManager;

		short m_i;
		short m_j;

		String m_name;

		short m_power;
		short m_size;
		short m_tileSize;
		short m_sizeInTiles;
		std::vector<TerrainTile*> m_tiles;

		float* m_heightfield;

		short m_bestLOD;
		short m_worstLOD;

		float m_west;
		float m_south;
		float m_width;
		float m_tileWidth;
		float m_bestLODPointSpacing;

		Ogre::VertexData m_vertexData;
		Ogre::HardwareVertexBufferSharedPtr m_pVertexBuffer;

		// the offset between a vertex and the one directly to the south
		short m_vertexBufferStride;

		Ogre::AxisAlignedBox m_boundingBox;

		Ogre::MaterialPtr m_pMaterial;


    // physics
    btHeightfieldTerrainShape* m_pCollisionShape;
    btMotionState* m_pMotionState;
    btRigidBody* m_pPhysicsBody;

	public:
		Terrain* GetTerrain() { return m_pTerrain; }
		__declspec(property(get=GetTerrain)) Terrain* Terrain;

		static String GetName(short i, short j); 
		static String GetName(unsigned int id); 
		String GetName(){ return m_name; }

		float GetWidth() const { return m_width; }
		__declspec(property(get=GetWidth)) float Width;

		float GetWest() const { return m_west; }
		__declspec(property(get=GetWest)) float WestX;

		float GetSouth() const { return m_south; }
		__declspec(property(get=GetSouth)) float SouthY;

		static unsigned int ID(short i, short j);
		static short IFromID(unsigned int id);
		static short JFromID(unsigned int id);

		TerrainPage(NonUniformTerrain::Terrain* pTerrain, const NonUniformTerrain::TerrainPageData& pageData);
		~TerrainPage();

    void initPhysics();
    btRigidBody* getPhysicsBody() const { return m_pPhysicsBody; } 
    __declspec(property(get=getPhysicsBody)) btRigidBody* PhysicsBody;


		unsigned int GetID() { return ID(m_i, m_j); }

		Ogre::SceneNode* GetSceneNode() { return m_pSceneNode; }

		// page indices
		short GetI() const { return m_i; }
		short GetJ() const { return m_j; }

		// the value n for a page 2^n+1 vertices across
		short GetPower() { return m_power; }

		// world space coordinates from sample indices
		float GetX(short p);
		float GetY(short q);
		inline float GetZ(short p, short q) { return m_heightfield[(p >> m_bestLOD) + ((q >> m_bestLOD) * m_size)]; }
		inline void SetZ(short p, short q, float z) {m_heightfield[(p >> m_bestLOD) + ((q >> m_bestLOD) * m_size)] = z; }

		Vector3 GetPoint(short p, short q) { return Vector3(GetX(p), GetY(q), GetZ(p,q)); }
		void GetNormal(short p, short q, Vector3& normal);

		/**
		 * Returns the highest detail level of detail available.
		 */
		short GetBestLOD() { return m_bestLOD; }

		/**
		 * Returns the lowest detail level of detail available.
		 */
		short GetWorstLOD() { return m_worstLOD; }

		/**
		 * Returns the true z value (ie at the best LOD)
		 */
		float GetHeight(float x, float y);

		/**
		 * Returns the y value of the point by examining the mesh at the specified LOD.
		 */
		float GetInterpolatedHeight(float x, float y, short lod);

		inline unsigned int GetVertexIndex(short p, short q) { return (p >> m_bestLOD) + ((q >> m_bestLOD) * m_vertexBufferStride); }

		void Create(const TerrainPageData& pageData);

		Ogre::VertexData* GetVertexData() { return &m_vertexData; }

		const Ogre::AxisAlignedBox& GetBoundingBox() const { return m_boundingBox; }

		const Ogre::MaterialPtr& GetMaterial() const { return m_pMaterial; }

		void AddedToTerrain(Ogre::SceneNode* pSceneNode, const std::vector<TerrainPage*>& neighbours, TerrainTileIndexDataManager* pTerrainTileIndexDataManager);
		void RemovedFromTerrain(const std::vector<TerrainPage*>& neighbours);

	private:
		void InitVertexData();
		void FillVertexBuffer();

		void AdjustHeightfieldToMatchNeighbours(const std::vector<TerrainPage*>& neighbours);
		void SetInterPageTileNeighbours(const std::vector<TerrainPage*>& neighbours);
		void UnsetInterPageTileNeighbours(const std::vector<TerrainPage*>& neighbours);

		void CreateTiles();
		void CreateTile(short i, short j);
		TerrainTile* GetTile(short i, short j) { return m_tiles[i + j * m_sizeInTiles]; }
		void SetTileNeighbours();

		void InitializeMaterial();
		String GetMaterialName();
		String GetLightMapTextureName();

	};

}
