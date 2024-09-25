#pragma once
#include<glm/glm.hpp>
#include<GLAD/glad.h>
#include<GLFW/glfw3.h>

#include<map>
#include<string>

struct BufferObject {
    unsigned int FBO = 0;
    unsigned int VAO = 0;
    unsigned int VBO = 0;
};

class Renderer
{
public:
    Renderer() {};
    ~Renderer() {};

    

	void CreateBufferObject(const std::string& name);
	void BindBuffer(const std::string& name);

    void RenderQuad(const std::string& name);
    void Render(const std::string& name, float * vertices, unsigned int sizeArray, unsigned int num_vertices, bool haveTexCoords);

private:
    BufferObject* GetBufferObject(const std::string& name);
	std::map<std::string, BufferObject> m_bufferMap;
};

