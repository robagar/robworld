#pragma once

#include "GeoLocation.h"
#include "MapCoordinate.h"

namespace Geo
{
	/**
	 * @brief 
	 * The Earths equatorial radius in metres.
	 */
	static const double c_earthRadiusMetres = 6378137.0;

	/**
	 * @brief 
	 * Base class for conversions between geographical (latitude/longitude) and cartesian coordinates.
	 * 
	 */
	class MapProjection
	{
	protected:
		MapCoordinate m_origin;

		MapProjection(const MapCoordinate& origin)
		: m_origin(origin)
		{
		}

	public:
		void SetOrigin(const MapCoordinate& origin) { m_origin = origin; }

		/**
		 * @brief 
		 * Converts lat/long to x,y
		 * 
		 * @param lat/long position
		 * @return x,y position
		 */
		virtual MapCoordinate LatLongToCoord(const GeoLocation&) PURE;


		/**
		 * @brief 
		 * Converts x,y to lat/long.
		 * 
		 * @param x,y position
		 * @return lat/long position
		 */
		virtual GeoLocation CoordToLatLong(const MapCoordinate&) PURE;
	};
}