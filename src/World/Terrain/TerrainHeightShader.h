#pragma once

namespace NonUniformTerrain
{
	struct TerrainHeightShaderMarkedHeight
	{
		float height;
		//ColourValue color;
		// TerrainHeightShaderColorInterpolation interpolation; 
		bool showLabel;
	};

	class TerrainHeightShader
	{
		String m_name;
		String m_textureName;
		float m_maxHeight;
		float m_vPerMetre;
		std::vector<TerrainHeightShaderMarkedHeight> m_heights;

	public: 
		TerrainHeightShader(const boost::filesystem::path& file);

		const String& GetName() const { return m_name; }
		__declspec(property(get=GetName)) const String& Name;

		const String& GetTextureName() const { return m_textureName; }
		__declspec(property(get=GetTextureName)) const String& TextureName;

		void SetCustomShaderParameters(Ogre::Renderable* pRenderable) const;

		const std::vector<TerrainHeightShaderMarkedHeight>& GetHeights() const { return m_heights; }
		__declspec(property(get=GetHeights)) const std::vector<TerrainHeightShaderMarkedHeight>& Heights;

		float GetVForHeight(float height) const { return clamp((height - m_maxHeight) * m_vPerMetre, 0.0f, 1.0f); }

	private:
		bool LoadFromFile(const boost::filesystem::path& file);
	};
}