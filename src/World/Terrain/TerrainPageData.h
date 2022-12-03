#pragma once;


namespace NonUniformTerrain
{
	enum TerrainPageStatus
	{
		TerrainPageStatus_OK,
		TerrainPageStatus_NoData
	};

	struct TerrainPageData
	{
		unsigned int ID;		
		TerrainPageStatus Status;

		short I;
		short J;

		short Power;
		short Size;

		int NumHeightSamples;
		float* HeightField;

		TerrainPageData(unsigned int id);
		~TerrainPageData();

		bool IsGood() const { return Status == TerrainPageStatus_OK; } 

		void Create(short power);

		float GetHeightAt(int i, int j) const { return HeightField[i + (j * Size)]; }
		void SetHeightAt(int i, int j, float h) { HeightField[i + (j * Size)] = h; }
	};
}