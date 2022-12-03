#include "../StdAfx.h"
#include "GeoPolygon.h"

using namespace Geo;

Polygon::Polygon()
{
}

const GeoLocation& Polygon::GetVertex(int index) const 
{
	// allow querying of end copy of first vertex 
	assert(index <= NumVertices);

	return m_vertices[index]; 
}

void Polygon::SetVertex(int index, const GeoLocation& vertex)
{
	assert(index < NumVertices);

	m_vertices[index] = vertex;

	// if first vertex, also set the end copy 
	if (index == 0)
		m_vertices[NumVertices] = vertex;

}

bool Polygon::InitializeFromKmlElement(const TiXmlElement& element)
{
	const TiXmlElement* pOuterBoundaryElement = element.FirstChildElement("outerBoundaryIs");
	if (pOuterBoundaryElement == NULL) return false;
	const TiXmlElement* pRingElement = pOuterBoundaryElement->FirstChildElement("LinearRing");
	if (pRingElement == NULL) return false;
	const TiXmlElement* pCoordinatesElement = pRingElement->FirstChildElement("coordinates");

	string coords = pCoordinatesElement->GetText();
	
	// replace newlines and carriage returns with spaces 
	replace(coords.begin(), coords.end(), '\r', ' ');
	replace(coords.begin(), coords.end(), '\n', ' ');

	// split coordinates string into tuples on spaces
	vector<string> tuples;
	split(tuples, coords, boost::algorithm::is_space(), boost::algorithm::token_compress_on);

	// read each longitude, latitude, altitude tuple
	foreach(string& tuple, tuples)
	{
		double longitude, latitude, altitude = 0; 
		istringstream is(tuple);
	
		// read "[longitude],[latitude]"
		is >> longitude;
		is.ignore(1);
		is >> latitude;
		
		// altitude value is optional
		if (!is.eof())
		{
			// read ",[altitude]"
			is.ignore(1);
			is >> altitude;
		}

		m_vertices.push_back(GeoLocation(longitude, latitude));
	}

	//// found any vertices at all?
	//if(m_vertices.empty())
	//	return false;

	//// the last vertex should be identical to the first - remove if it is so
	//if (m_vertices.front() == m_vertices.back())
	//	m_vertices.resize(m_vertices.size() - 1);

	//// must have at least three vertices remaining
	//if (m_vertices.size() < 3)
	//	return false;

	// must have at least four vertices
	if (m_vertices.size() < 4)
		return false;

	return true;
}