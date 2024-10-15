#include "WindowManager.h"

WindowManager::WindowManager()
{
	m_renderer = new Renderer();
	m_pbrBuffer = new PBR_BUFFER(512);
	glGenFramebuffers(1, &m_shadowMapFBO);
}

void WindowManager::AddRenderType(const std::string& name)
{
	m_renderer->CreateBufferObject(name);
}

void WindowManager::RenderCube(const std::string& name)
{
	m_renderer->RenderCube(name);
}

void WindowManager::RenderSphere(const std::string& name)
{
	m_renderer->RenderSphere(name);
}

void WindowManager::BindTexture(unsigned int& textureID, int renderSize, TextureType type)
{
	switch (type)
	{
	case EvironmentMap:
		m_pbrBuffer->BindEvironmentMapTexture(textureID, renderSize);
		break;
	case IrradianceMap:
		m_pbrBuffer->BindIrradianceTexture(textureID, renderSize);
		break;
	case PrefilterMap:
		m_pbrBuffer->BindPrefilterTexture(textureID, renderSize);
		break;
	case DepthMap:
		BindShadowMapTexture(textureID, renderSize);
		break;
	default:
		break;
	}
	
}

void WindowManager::RenderDepthTexture(unsigned int& textureID, Shader shader, int renderSize)
{

}


void WindowManager::RenderEvironmentMapTexture(unsigned int& textureID, Shader shader, int renderSize)
{
	m_pbrBuffer->BindFrameBuffer(renderSize);
	for (unsigned int i = 0; i < 6; ++i)
	{
		shader.SetMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, textureID, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		RenderCube("cube");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void WindowManager::RenderIrradianceTexture(unsigned int& textureID, Shader shader, int renderSize)
{

	m_pbrBuffer->BindFrameBuffer(32);

	for (unsigned int i = 0; i < 6; ++i)
	{
		shader.SetMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, textureID, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		RenderCube("cube");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
}

void WindowManager::RenderPrefilterTextures(unsigned int& textureID, Shader shader, int renderSize, int mipSize)
{
	m_pbrBuffer->BindFrameBuffer(renderSize);

	unsigned int maxMipLevels = mipSize;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// reisze framebuffer according to mip-level size.
		// mipmap -> 128 -> 64 -> 32 -> 16 -> 8
		unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
		unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
		m_pbrBuffer->RenderbufferStorage(mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		shader.SetFloat("roughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			shader.SetMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, textureID, mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			RenderCube("cube");
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void WindowManager::BindShadowMapTexture(unsigned int& textureID, int renderSize)
{
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, renderSize, renderSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureID, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


