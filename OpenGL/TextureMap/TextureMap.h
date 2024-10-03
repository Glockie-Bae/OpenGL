
#include<glm/glm.hpp>
#include<GLAD/glad.h>
#include<GLFW/glfw3.h>


#include<iostream>
#include<string>

struct TextureMap
{
	unsigned int albedoMap = 0;
	unsigned int normalMap = 0;
	unsigned int metallicMap = 0;
	unsigned int roughnessMap = 0;
	unsigned int aoMap = 0;
	TextureMap();

	TextureMap(unsigned int albedoMap, unsigned int normalMap, unsigned int metallicMap, unsigned int roughnessMap, unsigned int aoMap);
	TextureMap(const std::string& FilePath);

	void LoadTextureMap(const std::string& FilePath);
};

unsigned int load_image(const char* imageFile);

unsigned int load_hdr_image(const char* imageFile);