#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;
	// 保证ifstream对象可以抛出异常：
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		// 打开文件
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// 读取文件的缓冲内容到数据流
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// 关闭文件处理器
		vShaderFile.close();
		fShaderFile.close();
		// 转换数据流到string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

		// 打开gshader文件
		if (geometryPath != nullptr)
		{
			gShaderFile.open(geometryPath);
			std::stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;

	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	// 编译shaders
	unsigned int vertex, fragment;
	vertex = glCreateShader(GL_VERTEX_SHADER);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");

	unsigned int geometry;
	if (geometryPath != nullptr)
	{
		const char* gShaderCode = geometryCode.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		checkCompileErrors(geometry, "GEOMETRY");
	}

	// shader 程序
	m_ShaderProgram = glCreateProgram();
	glAttachShader(m_ShaderProgram, vertex);
	glAttachShader(m_ShaderProgram, fragment);
	if (geometryPath != nullptr)
		glAttachShader(m_ShaderProgram, geometry);
	glLinkProgram(m_ShaderProgram);
	checkCompileErrors(m_ShaderProgram, "PROGRAM");
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geometryPath != nullptr)
		glDeleteShader(geometry);
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

void Shader::SetVec2f(const std::string& name, glm::vec2 vec) const
{
	glUniform2f(glGetUniformLocation(m_ShaderProgram, name.c_str()), vec.x, vec.y);
}

void Shader::SetMaterial(const std::string& name, Material mateial) const
{
	SetVec3f(name + ".albedo", mateial.albedo);
	SetFloat(name + ".metallic", mateial.metallic);
	SetFloat(name + ".roughness", mateial.roughness);
	SetFloat(name + ".ao", mateial.ao);
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

void Shader::SetSpotLight(const std::string& name, SpotLight spotLight, bool spotLightSwitch) const
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

	SetBool("spotLightSwitch", spotLightSwitch);
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
	// 为当前绑定的纹理对象设置环绕、过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Shader::StencilTest(unsigned int VAO, unsigned int texture)
{
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);
	UseProgram();
	float scale = 1.1f;

	// cubes
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
	model = glm::scale(model, glm::vec3(scale, scale, scale));
	SetMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(scale, scale, scale));
	SetMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glEnable(GL_DEPTH_TEST);
}

void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
	// 判断是否vertex shader编译成功
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

Material::Material(glm::vec3 albedo, float metallic, float roughness, float ao)
{
	this->albedo = albedo;
	this->metallic = metallic;
	this->roughness = roughness;
	this->ao = ao;

	textureMap = TextureMap();
}

Material::Material(glm::vec3 albedo, float metallic, float roughness, float ao, const std::string& filePath)
{
	this->albedo = albedo;
	this->metallic = metallic;
	this->roughness = roughness;
	this->ao = ao;

	textureMap = TextureMap(filePath);
}

void Material::LoadTextureMap(const std::string& filePath)
{
	textureMap.LoadTextureMap(filePath);
}

