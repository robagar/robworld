#pragma once

namespace Geo
{
	struct MapCoordinate
	{
		double east;
		double north;

		MapCoordinate(double _east = 0.0, double _north = 0.0)
			: east(_east), north(_north)
		{
		}
	};
}