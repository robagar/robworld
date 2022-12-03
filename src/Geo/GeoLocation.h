#pragma once

namespace Geo
{
	/**
	 * @brief 
	 * Represents a position specified by latitude and longitude.
	 * 
	 */
	struct GeoLocation
	{
		/**
		 * @brief 
		 * The position longitude in degrees.
		 */
		double longitude;


		/**
		 * @brief 
		 * The position latitude in degrees.
		 */
		double latitude;

		/**
		 * @brief 
		 * Constructor.
		 * 
		 * @param _longitude longitude in degrees
		 * @param _latitude latitude in degrees
		 */
		GeoLocation(double _longitude = 0.0, double _latitude = 0.0)
			: longitude(_longitude), latitude(_latitude)
		{
		}

		/**
		 * @brief 
		 * Checks that the longitude is between -180 and +180 and that latitude between -90 and +90. 
		 * 
		 * @return true if both coords are in range
		 */
		const bool Verify()
		{
			return longitude >= -180.0 && longitude <= 180.0 
				&& latitude >= -90.0 && latitude <= 90.0; 
		}

		///**
		// * @brief 
		// * Convenience insertion operator for writing lat/long coordinates to a stream.
		// * 
		// * @param o
		// * @param l
		// * @return the stream
		// */
		//inline friend std::ostream& operator << (std::ostream& o, const GeoLocation& l )
		//{
		//	o << "long " << degminsec(l.longitude) << ", lat " << degminsec(l.latitude);
		//	return o;
		//}

		/**
		 * @brief 
		 * Compares this lat/long position with another pair and sets each to
		 * the comparison value if closer to +180° longitude or +90° latitude.
		 * 
		 * @param other
		 */
		inline void makeCeil(const GeoLocation& other)
		{
			if (other.longitude > longitude) longitude = other.longitude;
			if (other.latitude > latitude) latitude = other.latitude;
		}

		/**
		 * @brief 
		 * Compares this lat/long position with another pair and sets each to
		 * the comparison value if closer to -180° longitude or -90° latitude.
		 * 
		 * @param other
		 */
		inline void makeFloor(const GeoLocation& other)
		{
			if (other.longitude < longitude) longitude = other.longitude;
			if (other.latitude < latitude) latitude = other.latitude;
		}

	};


	/**
	 * @brief 
	 * Convenience insertion operator for writing lat/long coordinates to a stream.
	 * 
	 * @param o
	 * @param l
	 * @return the stream
	 */
	std::ostream& operator << (std::ostream& o, const GeoLocation& l );


	/**
	 * @brief 
	 * Convenience insertion operator for writing lat/long coordinates to a (wide char) stream.
	 * 
	 * @param o
	 * @param l
	 * @return the stream
	 */
	std::wostream& operator << (std::wostream& o, const GeoLocation& l );

  std::istream& operator >> (std::istream& is, GeoLocation& l);

}
