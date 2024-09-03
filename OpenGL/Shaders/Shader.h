#pragma once

#include <glad/glad.h>; // ����glad����ȡ���еı���OpenGLͷ�ļ�

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Material{

    // �������յĹ���
    // (0.0f, 0.0f, 1.0f) ��ʾֻ������ɫ����
	glm::vec3 ambient;
	glm::vec3 diffuse;
    glm::vec3 specular;
	float shininess;

    Material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess);
};

struct Light {

    // ��Դ����Ĺ���
    // (0.0f, 0.0f, 1.0f) ��ʾ��ֻ������ɫ����
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    Light(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
};

class Shader
{
public:

    // ��������ȡ��������ɫ��
    Shader(const char* vertexPath, const char* fragmentPath);
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

    // uniform vector
    void SetVec3f(const std::string& name, glm::vec3 vec) const;

	// uniform material
    void SetMaterial(const std::string& name, Material mateial) const;
    void SetLight(const std::string& name, Light light) const;

    
    // ɾ������
    void DeleteProgram();

    unsigned int GetID() const;

    // texture
	void BindTexture(unsigned int& textureID);

private:
	void checkCompileErrors(unsigned int shader, std::string type);
    unsigned int m_ShaderProgram;


};
