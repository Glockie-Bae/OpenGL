#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ��֤ifstream��������׳��쳣��
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		// ���ļ�
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// ��ȡ�ļ��Ļ������ݵ�������
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// �ر��ļ�������
		vShaderFile.close();
		fShaderFile.close();
		// ת����������string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;

	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	// ����shaders
	unsigned int vertex, fragment;
	vertex = glCreateShader(GL_VERTEX_SHADER);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	glCompileShader(fragment);
	checkCompileErrors(vertex, "VERTEX");

	// shader ����
	m_ShaderProgram = glCreateProgram();
	glAttachShader(m_ShaderProgram, vertex);
	glAttachShader(m_ShaderProgram, fragment);
	glLinkProgram(m_ShaderProgram);


	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::UseProgram()
{
	glUseProgram(m_ShaderProgram);
}

void Shader::SetBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(m_ShaderProgram, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(m_ShaderProgram, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(m_ShaderProgram, name.c_str()), value);
}

void Shader::Set4Float(const std::string& name, float v0, float v1, float v2, float v3) const
{
	glUniform4f(glGetUniformLocation(m_ShaderProgram, name.c_str()), v0, v1, v2, v3);
}

void Shader::SetMat4(const std::string& name, glm::mat4 trans) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_ShaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(trans));
}

void Shader::SetMat3(const std::string& name, glm::mat3 trans) const
{
	glUniformMatrix3fv(glGetUniformLocation(m_ShaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(trans));
}

void Shader::SetVec3f(const std::string& name, glm::vec3 vec) const
{
	glUniform3f(glGetUniformLocation(m_ShaderProgram, name.c_str()), vec.x, vec.y, vec.z);
}

void Shader::SetMaterial(const std::string& name, Material mateial) const
{
	SetVec3f(name + ".ambient", mateial.ambient);
	SetVec3f(name + ".diffuse", mateial.diffuse);
	SetVec3f(name + ".specular", mateial.specular);
	SetFloat(name + ".shininess", mateial.shininess);
}

void Shader::SetLight(const std::string& name, Light light) const
{
	SetVec3f(name + ".ambient", light.ambient);
	SetVec3f(name + ".diffuse", light.diffuse);
	SetVec3f(name + ".specular", light.specular);
}

void Shader::SetPointLight(const std::string& name, PointLight light) const
{
	SetVec3f(name + ".position", light.position);
	SetVec3f(name + ".ambient", light.ambient);
	SetVec3f(name + ".diffuse", light.diffuse);
	SetVec3f(name + ".specular", light.specular);
	SetFloat(name + ".constant", light.constant);
	SetFloat(name + ".linear", light.linear);
	SetFloat(name + ".quadratic", light.quadratic);
}

void Shader::SetDirLight(const std::string& name, DirLight light) const
{
	SetVec3f(name + ".direction", light.direction);
	SetVec3f(name + ".ambient", light.ambient);
	SetVec3f(name + ".diffuse", light.diffuse);
	SetVec3f(name + ".specular", light.specular);
}

void Shader::SetSpotLight(const std::string& name, SpotLight spotLight) const
{
	SetVec3f(name + ".position", spotLight.position);
	SetVec3f(name + ".direction", spotLight.direction);
	SetVec3f(name + ".ambient", spotLight.ambient);
	SetVec3f(name + ".diffuse", spotLight.diffuse);
	SetVec3f(name + ".specular", spotLight.specular);
	SetFloat(name + ".constant", spotLight.constant);
	SetFloat(name + ".linear", spotLight.linear);
	SetFloat(name + ".quadratic", spotLight.quadratic);
	SetFloat(name + ".cutOff", spotLight.cutOff);
	SetFloat(name + ".outerCutOff", spotLight.outerCutOff);
}

void Shader::DeleteProgram()
{
	glDeleteProgram(m_ShaderProgram);
}

unsigned int Shader::GetID() const
{
	return m_ShaderProgram;
}

void Shader::BindTexture(unsigned int& textureID)
{
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
	// �ж��Ƿ�vertex shader����ɹ�
	int  success;
	char infoLog[512];
	if (type == "PROGRAM") {
		// check for linking errors
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
	}
	else {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::" << type << "::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
	}

}

Material::Material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess)
{
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->shininess = shininess;
}

