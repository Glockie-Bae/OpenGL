#pragma once
#include"Light/Light.h"
#include<vector>

class LightManager {

public:

	void AddDirLight(DirLight light);
	void AddPointLight(PointLight light);
	void AddSpotLight(SpotLight light);

	int GetPointLightCount();

	DirLight GetDirLight(unsigned int index);
	PointLight GetPointLight(unsigned int index);
	SpotLight GetSpotLight(unsigned int index);

private:
	std::vector<DirLight> m_dirLights;
	std::vector<PointLight> m_pointLights;
	std::vector<SpotLight> m_spotLights;
};