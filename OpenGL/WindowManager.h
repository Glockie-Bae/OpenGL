#pragma once

#include"Render/Renderer.h"
#include"PBR/pbrBuffer.h"
#include"Camera.h"


class WindowManager {
public:
	WindowManager();

	void AddRenderType(const std::string& name);

	void RenderCube(const std::string& name);

	void RenderSphere(const std::string& name);

	void RenderEvironmentMapTexture(unsigned int& textureID, Shader shader, int renderSize);

	void RenderIrradianceTexture(unsigned int* textureID, Shader* shader, int renderSize);

	void RenderMipMapTextures(unsigned int& textureID, Shader shader, int renderSize, int mipSize);

	Renderer* m_renderer;

	PBR_BUFFER* m_pbrBuffer;
private:
	
};

