#pragma once

#include"Shaders/Shader.h"

#include<glm/glm.hpp>
#include<string>
#include<vector>


#define MAX_BONE_INFLUENCE 4

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;

    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture
{
	unsigned int id;
	std::string type;
    std::string path;
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