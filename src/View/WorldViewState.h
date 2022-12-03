#pragma once

struct WorldViewState
{
	String name;

	bool wireframe;

	bool showHUD;

	bool showTerrain;
	String terrainDepthShaderName;

	bool showFog;
	bool showSky;
	bool showOceanSurface;

	bool showUserData;

	WorldViewState()
		: name("default"),
			wireframe(false),
			showHUD(true),
			showTerrain(true),
			terrainDepthShaderName("default"),
			showFog(true),
			showSky(false),
			showOceanSurface(false),
			showUserData(false)
	{
	}
};
