#include "../StdAfx.h"
#include "GeoLocation.h"
#include "DegMinSec.h"

namespace Geo
{

  /**
   * @brief 
   * Convenience insertion operator for writing lat/long coordinates to a stream.
   * 
   * @param o
   * @param l
   * @return the stream
   */
  std::ostream& operator << (std::ostream& o, const Geo::GeoLocation& l )
  {
  	o << longitude_degminsec(l.longitude) << ", " << latitude_degminsec(l.latitude);
  	return o;
  }


  /**
   * @brief 
   * Convenience insertion operator for writing lat/long coordinates to a (wide char) stream.
   * 
   * @param o
   * @param l
   * @return the stream
   */
  std::wostream& operator << (std::wostream& o, const Geo::GeoLocation& l )
  {
  	o << longitude_degminsec(l.longitude) << ", " << latitude_degminsec(l.latitude);
  	return o;
  }

  std::istream& operator >> (std::istream& is, GeoLocation& l)
  {
  	char comma;
  	is >> std::ws >> l.longitude;
  	is >> std::ws >> comma;
  	is >> std::ws >> l.latitude;
  	return is;
  }
}