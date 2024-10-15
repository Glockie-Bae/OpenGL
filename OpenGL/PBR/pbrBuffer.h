#pragma once

#include<glm/glm.hpp>
#include<GLAD/glad.h>
#include<GLFW/glfw3.h>

#include"Shaders/Shader.h"


class PBR_BUFFER {
public:
	PBR_BUFFER(int renderSize);

	unsigned int* GetCaptureFBO();
	unsigned int* GetCaptureRBO();

	void BindIrradianceTexture(unsigned int& textureID, int renderSize);

	void BindPrefilterTexture(unsigned int& textureID, int renderSize);

	void BindBRDFTexture(unsigned int& textureID, int renderSize);

	void BindEvironmentMapTexture(unsigned int& textureID, int renderSize);

	void BindFrameBuffer(int renderSize);

	void BindDepthMapTexture(unsigned int& textureID, int renderSize);

	void RenderbufferStorage(int width, int height);

private:
	// frame buffer object
	unsigned int m_captureFBO;
	// frame render buffer object
	unsigned int m_captureRBO;


};