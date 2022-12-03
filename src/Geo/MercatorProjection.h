#pragma once

#include "MapProjection.h"

namespace Geo
{

	/**
	 * @brief 
	 * For mapping lat/long points to east/north coordinates using the Mercator projection.
	 * 
	 * @see CMapProjection
	 */
	class MercatorProjection : public MapProjection
	{
	public:
		MercatorProjection(const MapCoordinate& origin);

		virtual MapCoordinate LatLongToCoord(const Geo::GeoLocation& l);
		virtual Geo::GeoLocation CoordToLatLong(const MapCoordinate& c);
	};
}