#pragma once

#include <glad/glad.h>; // 包含glad来获取所有的必须OpenGL头文件

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Material{

    // 物体吸收的光线
    // (0.0f, 0.0f, 1.0f) 表示只吸收蓝色光线
	glm::vec3 ambient;
	glm::vec3 diffuse;
    glm::vec3 specular;
	float shininess;

    Material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess);
};

struct Light {

    // 光源发射的光线
    // (0.0f, 0.0f, 1.0f) 表示光只发射蓝色光线
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    Light(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
};

class Shader
{
public:

    // 构造器读取并构建着色器
    Shader(const char* vertexPath, const char* fragmentPath);
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

    // uniform vector
    void SetVec3f(const std::string& name, glm::vec3 vec) const;

	// uniform material
    void SetMaterial(const std::string& name, Material mateial) const;
    void SetLight(const std::string& name, Light light) const;

    
    // 删除程序
    void DeleteProgram();

    unsigned int GetID() const;

    // texture
	void BindTexture(unsigned int& textureID);

private:
	void checkCompileErrors(unsigned int shader, std::string type);
    unsigned int m_ShaderProgram;


};
