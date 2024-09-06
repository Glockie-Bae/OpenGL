#pragma once

#include"Shaders/Shader.h"



#include<glm/glm.hpp>
#include<string>
#include<vector>


struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 TexCoords;
};

struct Texture
{
	unsigned int id;
	std::string type;
};

class Mesh {
public:
    /*  网格数据  */
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    /*  函数  */
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void Draw(Shader shader);
private:
    /*  渲染数据  */
    unsigned int m_VAO, m_VBO, m_IBO;
    /*  函数  */
    void SetupMesh();
};