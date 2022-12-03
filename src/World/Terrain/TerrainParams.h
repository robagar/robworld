#pragma once

namespace NonUniformTerrain
{
	struct TerrainParams
	{
		short ZeroLODPagePower;
		short WorstLODPagePower;

		short WorstLOD;

		// the value n if a page is 2^n tiles across 
		// (not the same as tilePower, which relates to the number of *vertices* in a tile)
		short PageTilingPower;

		short ZeroLODPageSize;
		float PageWidth;
		short PageSizeInTiles;
		short TilePower;
		short TileSize;

		bool VertexLightingEnabled;
		bool LightMapEnabled;


		/////////////////////////////////////////////////////////////////////////////////////////////
		
		TerrainParams(const Config& config);

		short GetLOD(short power) const { return ZeroLODPagePower - power; }

		// returns the coordinate of the northern- or western-most point of a tile
		short GetTilePointCoordinate(short i) const { return i * (TileSize - 1); } 

		float GetBestLODPointSpacing() const { return PageWidth / (1 << ZeroLODPagePower); }

		short GetPageI(float x) const;
		short GetPageJ(float y) const;

		int GetPageIDAt(float x, float y) const; 

		float GetPageX(short i) const;
		float GetPageY(short j) const;
    Vector2 GetPageXY(short i, short j) const;
		
		inline short GetPageBestLOD(short pagePower) const { return ZeroLODPagePower - pagePower; }
		inline short GetPageSize(short pagePower) const { return (1 << pagePower) + 1; } 
		
		inline short GetVertexBufferStride(short pagePower) const  { return GetPageSize(pagePower); } 
		inline unsigned int GetVertexIndex(short pagePower, short p, short q) const 
		{
			short l = GetPageBestLOD(pagePower);
			return (p >> l) + ((q >> l) * GetVertexBufferStride(pagePower)); 
		}
	};
}