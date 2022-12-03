#pragma once
#include "GeoLocation.h"

namespace Geo
{
	class Polygon
	{
		std::vector<GeoLocation> m_vertices;

	public:
		Polygon();

		const GeoLocation& GetVertex(int index) const;
		void SetVertex(int index, const GeoLocation& vertex);

		const int GetNumVertices() const { return (int)m_vertices.size() - 1; }
		void SetNumVertices(const int n) { m_vertices.resize(n + 1); }
		__declspec(property(get=GetNumVertices, put=SetNumVertices)) const int NumVertices;

		bool InitializeFromKmlElement(const TiXmlElement& element);
	};
}