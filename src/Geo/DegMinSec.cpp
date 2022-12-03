#include "../StdAfx.h"
#include "DegMinSec.h"

std::ostream& degminsec::operator () (std::ostream& o) const
{
	double degrees, minutes, seconds;
	double m = fabs(modf(angle, &degrees)) * 60.0;
	seconds = modf(m, &minutes) * 60.0;

	double s = round(seconds, secondsDecimals);
	if (s >= 60.0)
	{
		seconds = 0.0;
		minutes += 1.0;
	}
	if (minutes >= 60.0)
	{
		minutes = 0.0;
		degrees += angle >= 0 ? 1.0 : -1.0; 
	}

	o << std::fixed << std::setprecision(0) << degrees << '°' 
		<< std::fixed << std::setprecision(0) << minutes << '\'' 
		<< std::fixed << std::setprecision(secondsDecimals) << seconds << '"';
	return o;
}

std::wostream& degminsec::operator () (std::wostream& o) const
{
	double degrees, minutes, seconds;
	double m = fabs(modf(angle, &degrees)) * 60.0;
	seconds = modf(m, &minutes) * 60.0;

	double s = round(seconds, secondsDecimals);
	if (s >= 60.0)
	{
		seconds = 0.0;
		minutes += 1.0;
	}
	if (minutes >= 60.0)
	{
		minutes = 0.0;
		degrees += angle >= 0 ? 1.0 : -1.0; 
	}

	o << std::fixed << std::setprecision(0) << degrees << '°' 
		<< std::fixed << std::setprecision(0) << minutes << '\'' 
		<< std::fixed << std::setprecision(secondsDecimals) << seconds << '"';
	return o;
}

// degminsec stream manipulator
std::ostream& operator << (std::ostream& o, degminsec d)
{
	return d(o);
}

// degminsec wide char stream manipulator
std::wostream& operator << (std::wostream& o, degminsec d)
{
	return d(o);
}


////////////////////////////////////////////////////////////////////////////////////////////////


std::ostream& longitude_degminsec::operator () (std::ostream& o) const
{
	double degrees, minutes, seconds;
	double m = fabs(modf(longitude, &degrees)) * 60.0;
	seconds = modf(m, &minutes) * 60.0;

	double s = round(seconds, secondsDecimals);
	if (s >= 60.0)
	{
		seconds = 0.0;
		minutes += 1.0;
	}
	if (minutes >= 60.0)
	{
		minutes = 0.0;
		degrees += longitude >= 0 ? 1.0 : -1.0; 
	}

	o << std::fixed << std::setprecision(0) << fabs(degrees) << '°' 
		<< std::fixed << std::setprecision(0) << minutes << '\'' 
		<< std::fixed << std::setprecision(secondsDecimals) << seconds << '"'
		<< (longitude < 0 ? 'W' : 'E');
	return o;
}

std::wostream& longitude_degminsec::operator () (std::wostream& o) const
{
	double degrees, minutes, seconds;
	double m = fabs(modf(longitude, &degrees)) * 60.0;
	seconds = modf(m, &minutes) * 60.0;

	double s = round(seconds, secondsDecimals);
	if (s >= 60.0)
	{
		seconds = 0.0;
		minutes += 1.0;
	}
	if (minutes >= 60.0)
	{
		minutes = 0.0;
		degrees += longitude >= 0 ? 1.0 : -1.0; 
	}

	o << std::fixed << std::setprecision(0) << fabs(degrees) << '°' 
		<< std::fixed << std::setprecision(0) << minutes << '\'' 
		<< std::fixed << std::setprecision(secondsDecimals) << seconds << '"'
		<< (longitude < 0 ? 'W' : 'E');
	return o;
}

// degminsec stream manipulator
std::ostream& operator << (std::ostream& o, longitude_degminsec d)
{
	return d(o);
}

// degminsec wide char stream manipulator
std::wostream& operator << (std::wostream& o, longitude_degminsec d)
{
	return d(o);
}


////////////////////////////////////////////////////////////////////////////////////////////////


std::ostream& latitude_degminsec::operator () (std::ostream& o) const
{
	double degrees, minutes, seconds;
	double m = fabs(modf(latitude, &degrees)) * 60.0;
	seconds = modf(m, &minutes) * 60.0;

	double s = round(seconds, secondsDecimals);
	if (s >= 60.0)
	{
		seconds = 0.0;
		minutes += 1.0;
	}
	if (minutes >= 60.0)
	{
		minutes = 0.0;
		degrees += latitude >= 0 ? 1.0 : -1.0; 
	}

	o << std::fixed << std::setprecision(0) << fabs(degrees) << '°' 
		<< std::fixed << std::setprecision(0) << minutes << '\'' 
		<< std::fixed << std::setprecision(secondsDecimals) << seconds << '"'
		<< (latitude < 0 ? 'S' : 'N');
	return o;
}

std::wostream& latitude_degminsec::operator () (std::wostream& o) const
{
	double degrees, minutes, seconds;
	double m = fabs(modf(latitude, &degrees)) * 60.0;
	seconds = modf(m, &minutes) * 60.0;

	double s = round(seconds, secondsDecimals);
	if (s >= 60.0)
	{
		seconds = 0.0;
		minutes += 1.0;
	}
	if (minutes >= 60.0)
	{
		minutes = 0.0;
		degrees += latitude >= 0 ? 1.0 : -1.0; 
	}

	o << std::fixed << std::setprecision(0) << fabs(degrees) << '°' 
		<< std::fixed << std::setprecision(0) << minutes << '\'' 
		<< std::fixed << std::setprecision(secondsDecimals) << seconds << '"'
		<< (latitude < 0 ? 'S' : 'N');
	return o;
}

// degminsec stream manipulator
std::ostream& operator << (std::ostream& o, latitude_degminsec d)
{
	return d(o);
}

// degminsec wide char stream manipulator
std::wostream& operator << (std::wostream& o, latitude_degminsec d)
{
	return d(o);
}
