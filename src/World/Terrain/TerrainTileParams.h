#pragma once

#include "TerrainTileNeighbour.h"

#define UNKNOWN_LOD -1

namespace NonUniformTerrain
{
	class TerrainPage;

	struct TerrainTileParams
	{
		short PagePower;

		// tile coordinates in page
		short I;
		short J;

		short LOD; 

		short NorthLOD;
		short EastLOD;
		short SouthLOD;
		short WestLOD;

		TerrainTileParams(short pagePower, short i, short j);

		bool operator < (const TerrainTileParams& rhs) const
		{
			if (PagePower < rhs.PagePower) return true;
			if (PagePower > rhs.PagePower) return false;

			if (I < rhs.I) return true;
			if (I > rhs.I) return false;

			if (J < rhs.J) return true;
			if (J > rhs.J) return false;

			if (LOD < rhs.LOD) return true;
			if (LOD > rhs.LOD) return false;

			if (NorthLOD < rhs.NorthLOD) return true;
			if (NorthLOD > rhs.NorthLOD) return false;

			if (EastLOD < rhs.EastLOD) return true;
			if (EastLOD > rhs.EastLOD) return false;

			if (SouthLOD < rhs.SouthLOD) return true;
			if (SouthLOD > rhs.SouthLOD) return false;

			if (WestLOD < rhs.WestLOD) return true;
			return false;
		}
		//bool operator == (const TerrainTileParams& lhs, const TerrainTileParams& rhs) const;

		inline bool MustStitchNorth() const { return NorthLOD > LOD; }
		inline bool MustStitchEast() const { return EastLOD > LOD; }
		inline bool MustStitchSouth() const { return SouthLOD > LOD; }
		inline bool MustStitchWest() const { return WestLOD > LOD; }
	};

	//bool operator == (const TerrainTileParams& lhs, const TerrainTileParams& rhs) const
	//{
	//	return PagePower == rhs.PagePower
	//				&& I == rhs.I && J == rhs.J
	//				&& LOD == rhs.LOD
	//				&& NorthLOD == rhs.NorthLOD
	//				&& EastLOD == rhs.EastLOD
	//				&& SouthLOD == rhs.SouthLOD
	//				&& WestLOD == rhs.WestLOD;
	//}
}