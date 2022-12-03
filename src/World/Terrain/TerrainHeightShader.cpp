#include "../../StdAfx.h"
#include "TerrainHeightShader.h"
#include "../../Misc/Config.h"

using namespace NonUniformTerrain;
using namespace boost;

// TODO: find a better way of doing this
#define MAX_HEIGHT_PARAM_INDEX 2
#define V_PER_METRE_PARAM_INDEX 3


TerrainHeightShader::TerrainHeightShader(const boost::filesystem::path& file)
{
	LoadFromFile(file);
}

bool TerrainHeightShader::LoadFromFile(const boost::filesystem::path& file)
{
	Config config(file);

	m_name = config.GetSetting<String>("name");
	m_textureName = config.GetSetting<String>("textureName");
	m_maxHeight = config.GetSetting<float>("maxHeight");
	m_vPerMetre = config.GetSetting<float>("vPerMetre");

	foreach(String& line, config.Data)
	{
		Ogre::StringVector values = StringUtil::split(line, ",");
		foreach(String& value, values)
			trim(value);

		TerrainHeightShaderMarkedHeight m;
		m.height = StringConverter::parseReal(values[0]);
		m.showLabel = StringConverter::parseBool(values[3]);		
		m_heights.push_back(m);
	}

	return true;
}

void TerrainHeightShader::SetCustomShaderParameters(Ogre::Renderable* pRenderable) const
{
	pRenderable->setCustomParameter(MAX_HEIGHT_PARAM_INDEX, Vector4(m_maxHeight));
	pRenderable->setCustomParameter(V_PER_METRE_PARAM_INDEX, Vector4(m_vPerMetre));
}