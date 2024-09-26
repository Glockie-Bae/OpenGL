#include"TextureMap.h"

#include"stb/stb_image.h"

TextureMap::TextureMap()
{

}

TextureMap::TextureMap(unsigned int albedoMap, unsigned int normalMap, unsigned int metallicMap, unsigned int roughnessMap, unsigned int aoMap)
{
	this->albedoMap = albedoMap;
	this->normalMap = normalMap;
	this->metallicMap = metallicMap;
	this->roughnessMap = roughnessMap;
	this->aoMap = aoMap;
}

TextureMap::TextureMap(const std::string& FilePath)
{
    albedoMap = load_image((FilePath + std::string("/albedo.png")).c_str());
    normalMap = load_image((FilePath + std::string("/normal.png")).c_str());
    metallicMap = load_image((FilePath + std::string("/metallic.png")).c_str());
    roughnessMap = load_image((FilePath + std::string("/roughness.png")).c_str());
    aoMap = load_image((FilePath + std::string("/ao.png")).c_str());
}

void TextureMap::LoadTextureMap(const std::string& FilePath)
{
    albedoMap = load_image((FilePath + std::string("/albedo.png")).c_str());
    normalMap = load_image((FilePath + std::string("/normal.png")).c_str());
    metallicMap = load_image((FilePath + std::string("/metallic.png")).c_str());
    roughnessMap = load_image((FilePath + std::string("/roughness.png")).c_str());
    aoMap = load_image((FilePath + std::string("/ao.png")).c_str());
}

unsigned int load_image(const char* imageFile) {
    // 加载并生成纹理
    unsigned int texTureID;
    glGenTextures(1, &texTureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(imageFile, &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format;
        GLenum internalFormat;
        if (nrChannels == 1) {
            format = GL_RED;
            internalFormat = GL_RED;
        }
        if (nrChannels == 3) {
            format = GL_RGB;
            internalFormat = GL_RGB16F;
        }
        if (nrChannels == 4) {
            format = GL_RGBA;
            internalFormat = GL_RGBA16F;
        }


        glBindTexture(GL_TEXTURE_2D, texTureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    return texTureID;

}