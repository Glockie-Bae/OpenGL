#pragma once

#include"Render/Renderer.h"
#include"PBR/pbrBuffer.h"
#include"Camera.h"

enum TextureType
{
	EvironmentMap, IrradianceMap, PrefilterMap
};

class WindowManager {
public:
	WindowManager();

	void AddRenderType(const std::string& name);

	void RenderCube(const std::string& name);

	void RenderSphere(const std::string& name);

	void BindTexture(unsigned int& textureID, int renderSize, TextureType type);


	void RenderEvironmentMapTexture(unsigned int& textureID, Shader shader, int renderSize);

	void RenderIrradianceTexture(unsigned int& textureID, Shader shader, int renderSize);

	void RenderPrefilterTextures(unsigned int& textureID, Shader shader, int renderSize, int mipSize);

	Renderer* m_renderer;

	PBR_BUFFER* m_pbrBuffer;
private:
	
};
