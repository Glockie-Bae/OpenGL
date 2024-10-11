#pragma once

#include <glad/glad.h>; // 包含glad来获取所有的必须OpenGL头文件

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

    // 物体吸收的光线
    // (0.0f, 0.0f, 1.0f) 表示只吸收蓝色光线
    glm::vec3 albedo;           // 颜色
    float metallic;             // 金属度
	float roughness;            // 粗糙度
    float ao;                   // 环境光遮蔽



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

    // 构造器读取并构建着色器
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
    // 使用/激活程序
    void UseProgram();
    // uniform工具函数
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
    
    // 删除程序
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
