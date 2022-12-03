#pragma once

#include "TerrainPage.h"

namespace NonUniformTerrain
{
	class TerrainPageCollection
	{
		std::map<int, TerrainPage*> m_pages;

	public:
		TerrainPageCollection();
		~TerrainPageCollection();

		void AddPage(TerrainPage* pTerrainPage);
		//TerrainPage* RemovePage(int i, int j);
		bool Contains(int id);
		bool Contains(short i, short j);
		TerrainPage* GetPage(short i, short j);
	};
}
