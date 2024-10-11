#pragma once

#include <glad/glad.h>; // ����glad����ȡ���еı���OpenGLͷ�ļ�

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include"Light/Light.h"
#include"TextureMap/TextureMap.h"

struct Material{

    // �������յĹ���
    // (0.0f, 0.0f, 1.0f) ��ʾֻ������ɫ����
    glm::vec3 albedo;           // ��ɫ
    float metallic;             // ������
	float roughness;            // �ֲڶ�
    float ao;                   // �������ڱ�



    TextureMap textureMap;

    Material() {}
    Material(glm::vec3 albedo, float metallic, float roughness, float ao);
    Material(glm::vec3 albedo, float metallic, float roughness, float ao, const std::string& filePath);
    Material(const char* filePath, bool tga = false);

    void LoadTextureMap(const std::string& filePath, bool tga = false);
};


class Shader
{
public:

    // ��������ȡ��������ɫ��
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
    // ʹ��/�������
    void UseProgram();
    // uniform���ߺ���
    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
	void Set4Float(const std::string& name, float v0, float v1, float v2, float v3) const;
    
    // uniform matrix
    void SetMat4(const std::string& name, glm::mat4 trans) const;
    void SetMat3(const std::string& name, glm::mat3 trans) const;

    void SetMaterial(const std::string& name, Material material) const;
    // uniform vector
    void SetVec3f(const std::string& name, glm::vec3 vec) const;
    void SetVec2f(const std::string& name, glm::vec2 vec) const;

	// uniform material
    void SetMaterialTexture(Material mateial);

    // uniform light
    void SetLight(const std::string& name, Light light) const;
    void SetPointLight(const std::string& name, PointLight light) const;
    void SetDirLight(const std::string& name, DirLight light) const;
    void SetSpotLight(const std::string& name, SpotLight spotLight, bool spotLightSwitch) const;


    void ActivateTexture(unsigned int& textureBuffer, GLuint GL_TEXTURE_TYPE);
    
    // ɾ������
    void DeleteProgram();

    unsigned int GetID() const;

    // texture
	void BindTexture(unsigned int& textureID);

    //GL_STENCIL_TEST
	void StencilTest(unsigned int VAO, unsigned int texture);


private:
	void checkCompileErrors(unsigned int shader, std::string type);
    unsigned int m_ShaderProgram;

    unsigned int m_textureID = 0;

    
};
