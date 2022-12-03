#pragma once

/**
 * @brief 
 * iostream manipulator for formatting angles as degrees, minutes and seconds. 
 */
class degminsec
{
	double angle;
	int secondsDecimals;
public:
	degminsec(double _angle, int _secondsDecimals = 0) 
		: angle(_angle), secondsDecimals(_secondsDecimals) {}

	std::ostream& operator () (std::ostream& o) const;
	std::wostream& operator () (std::wostream& o) const;
};

/**
 * @brief 
 * Insertion operator for angles in degree, minute, second format.
 */
std::ostream& operator << (std::ostream& o, degminsec d);

/**
 * @brief 
 * Insertion operator for angles in degree, minute, second format.
 */
std::wostream& operator << (std::wostream& o, degminsec d);

/**
 * @brief 
 * iostream manipulator for formatting longitudes as degrees, minutes and seconds. 
 */
class longitude_degminsec
{
	double longitude;
	int secondsDecimals;

public:
	longitude_degminsec(double _longitude, int _secondsDecimals = 0) 
		: longitude(_longitude), secondsDecimals(_secondsDecimals){}

	std::ostream& operator () (std::ostream& o) const;
	std::wostream& operator () (std::wostream& o) const;
};


/**
 * @brief 
 * Insertion operator for longitudes in degree, minute, second format.
 */
std::ostream& operator << (std::ostream& o, longitude_degminsec d);

/**
 * @brief 
 * Insertion operator for longitudes in degree, minute, second format.
 */
std::wostream& operator << (std::wostream& o, longitude_degminsec d);

/**
 * @brief 
 * iostream manipulator for formatting latitudes as degrees, minutes and seconds. 
 */
class latitude_degminsec
{
	double latitude;
	int secondsDecimals;

public:
	latitude_degminsec(double _latitude, int _secondsDecimals = 0) 
		: latitude(_latitude), secondsDecimals(_secondsDecimals){}

	std::ostream& operator () (std::ostream& o) const;
	std::wostream& operator () (std::wostream& o) const;
};


/**
 * @brief 
 * Insertion operator for latitudes in degree, minute, second format.
 */
std::ostream& operator << (std::ostream& o, latitude_degminsec d);

/**
 * @brief 
 * Insertion operator for latitudes in degree, minute, second format.
 */
std::wostream& operator << (std::wostream& o, latitude_degminsec d);