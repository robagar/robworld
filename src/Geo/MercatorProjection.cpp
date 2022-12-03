#include "../StdAfx.h"
#include "MercatorProjection.h"

using namespace Geo;

MercatorProjection::MercatorProjection(const MapCoordinate& origin)
: MapProjection(origin)
{
}

MapCoordinate MercatorProjection::LatLongToCoord(const GeoLocation& l)
{
	MapCoordinate c;

	c.east = c_earthRadiusMetres * deg2rad(l.longitude) - m_origin.east;
	c.north = c_earthRadiusMetres * log(tan((pi/4.0) + (deg2rad(l.latitude)/2.0))) - m_origin.north;

	return c;		
}

GeoLocation MercatorProjection::CoordToLatLong(const MapCoordinate& c)
{
	GeoLocation l;

	l.longitude = rad2deg((c.east + m_origin.east) / c_earthRadiusMetres);
	l.latitude = rad2deg(2.0 * atan(exp((c.north + m_origin.north) / c_earthRadiusMetres)) - pi / 2.0);

	return l;
}

