#include "LightManager.h"

void LightManager::AddDirLight(DirLight* light)
{
	m_dirLights.push_back(light);
}

void LightManager::AddPointLight(PointLight light)
{
	m_pointLights.push_back(light);
}

void LightManager::AddSpotLight(SpotLight light)
{
	m_spotLights.push_back(light);
}

int LightManager::GetPointLightCount()
{
	return m_pointLights.size();
}

DirLight* LightManager::GetDirLight(unsigned int index)
{
	return m_dirLights[index];
}

PointLight LightManager::GetPointLight(unsigned int index)
{
	return m_pointLights[index];
}

SpotLight LightManager::GetSpotLight(unsigned int index)
{
	return m_spotLights[index];
}
