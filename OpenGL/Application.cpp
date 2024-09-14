#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"
#include"Shaders/Shader.h"
#include"Camera.h"

#include"glm/vec3.hpp"
#include"Data.h"
#include"Light/Light.h"
#include"Light/LightManager.h"
#include"Model/Model.h"

#include"Mesh/Mesh.h"

#include <iostream>
#include<sstream>
#include<vector>
#include<map>

#define STB_IMAGE_IMPLEMENTATION
#include"stb/stb_image.h"



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void sroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int load_image(const char* imageFile);
unsigned int loadCubemap(std::vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 1080;

float currentTime = 0;
float lastTime = 0;
bool firstMouse = true;
Camera camera;

bool right_mouse_pressed = false;

glm::vec3 lightPos(1.2f, 0.58f, 2.0f);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, sroll_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    
    Shader shader("Shaders/shaderSource/Vertex.shader", "Shaders/shaderSource/Fragment.shader");
    Shader lightShader("Shaders/shaderSource/LightVertexShader.shader", "Shaders/shaderSource/LightFragmentShader.Shader");
    Shader skyBoxShader("Shaders/shaderSource/SkyBoxVertex.shader", "Shaders/shaderSource/SkyBoxFragment.shader");
    Shader planeShader("Shaders/shaderSource/DepthTestVS.shader", "Shaders/shaderSource/DepthTestFS.shader");
    Shader modelShader("Shaders/shaderSource/ModelVertexShader.shader", "Shaders/shaderSource/ModelFragmentShader.shader");

	Model ourModel("res/nanosuit/nanosuit.obj");


	// 顶点缓冲对象(Vertex Buffer Objects, VBO) 管理顶点数组 (顶点数组对象(Vertex Array Objects, VAO))
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(VAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));


    // 光源
    unsigned int lightVAO, lightVBO;
	glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glGenBuffers(1, &lightVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// VBO 存放箱子的位置数据
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // sky box VAO VBO;
    unsigned int skyBoxVAO, skyBoxVBO;
    glGenVertexArrays(1, &skyBoxVAO);
    glGenBuffers(1, &skyBoxVBO);
    glBindVertexArray(skyBoxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyBoxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));




    std::vector<std::string> faces
    {
           "res/skybox/right.jpg",
           "res/skybox/left.jpg",
           "res/skybox/top.jpg",
           "res/skybox/bottom.jpg",
           "res/skybox/front.jpg",
           "res/skybox/back.jpg"
    };

    


	unsigned int skyBoxTexture = loadCubemap(faces);
    skyBoxShader.UseProgram();
    skyBoxShader.SetInt("skybox", 0);

    shader.UseProgram();
    shader.SetInt("skybox", 1);

    unsigned int metalTexture = load_image("res/metal.jpg");
    planeShader.UseProgram();
    planeShader.SetInt("planeTexture", 2);


	// ImGui 初始化
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

    float size = 0.1f;
    //glm::vec3(1.2f, 0.58f, 2.0f)
    Material material(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), 128.0f);


    LightManager lightManager;
    lightManager.AddDirLight(new DirLight(glm::vec3(0.2f), glm::vec3(0.5f), glm::vec3(0.5f), glm::vec3(0.0f, 0.0f, -2.0f)));

    lightManager.AddSpotLight(new SpotLight(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(1.0f), camera.GetPos(), camera.GetFront(), 12.5f, 15.0f, 1.0f, 0.09f, 0.032f));
    for (int i = 0; i < 1; i++) {
        lightManager.AddPointLight(new PointLight(glm::vec3(0.2f), glm::vec3(1.0f), glm::vec3(1.0f), pointLightPositions[i], 1.0f, 0.09f, 0.032f));
    }

    bool spotLightSwitch = true;
    bool isStencilTest = false;
    bool glass = true;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // render
        // ------

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        // 清楚深度缓冲
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        camera.KeyboardMoveCamera(window, deltaTime);

        glm::mat4 model(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.GetFOV()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // model shader
        modelShader.UseProgram();
        model = glm::translate(model, glm::vec3(-1.5f, -0.5f, 0.0f));
        modelShader.SetMat4("model", model);
        modelShader.SetMat4("view", view);
        modelShader.SetMat4("projection", projection);
        modelShader.SetVec3f("cameraPos", camera.GetPos());
        modelShader.SetFloat("modelSize", size);
        ourModel.Draw(modelShader);

        model = glm::mat4(1.0f);
        // plane shader
        planeShader.UseProgram();
        glBindVertexArray(planeVAO);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, metalTexture);
        planeShader.SetMat4("model", model);
        planeShader.SetMat4("view", view);
        planeShader.SetMat4("projection", projection);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // cube shader
        shader.UseProgram();
        shader.SetMaterial("material", material);
        shader.SetMat4("view", view);
        shader.SetMat4("projection", projection);
        shader.SetVec3f("viewPos", camera.GetPos());

        // cubes
        glBindVertexArray(VAO);
        // model shader texture setting
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTexture);
        shader.SetMat4("model", model);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    
        model = glm::mat4(1.0f);       
        model = glm::translate(model, glm::vec3(1.5f, 0.0f, -0.5f));
        shader.SetMat4("model", model);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);



        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyBoxShader.UseProgram();

        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyBoxShader.SetMat4("view", view);
        skyBoxShader.SetMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyBoxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0); 
        glDepthFunc(GL_LESS); // set depth function back to default


        

        shader.UseProgram();
        // model shader light setting
        shader.SetDirLight("dirLight", *lightManager.GetDirLight(0));
		shader.SetInt("pointLightNumber", lightManager.GetPointLightCount());
        for (int i = 0; i < lightManager.GetPointLightCount(); i++) {
            char* name = new char[10];
            sprintf(name, "pointLight[%d]", i);
            shader.SetPointLight(name, *lightManager.GetPointLight(i));
        }
        // setting spotlight
        shader.SetSpotLight("spotLight", *lightManager.GetSpotLight(0), spotLightSwitch);
        (*lightManager.GetSpotLight(0)).Update(camera.GetPos(), camera.GetFront());



        // light shader
        // setting light cube
        lightShader.UseProgram();

		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.GetFOV()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        lightShader.SetMat4("view", view);
        lightShader.SetMat4("projection", projection);
        lightShader.SetFloat("size", size);

        for (int i = 0; i < lightManager.GetPointLightCount(); i++) {
            glm::mat4 lightModel = glm::mat4(1.0f);
            lightModel = glm::translate(lightModel, (*lightManager.GetPointLight(i)).position);
            lightModel = glm::scale(lightModel, glm::vec3(0.2f)); // a smaller cube
            lightShader.SetMat4("model", lightModel);
            glBindVertexArray(lightVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // ImGui Optional
        ImGui::Begin("Material");
        ImGui::SliderFloat3("Material Ambient", &material.ambient[0], 0.0f, 1.0f);
        ImGui::SliderFloat3("Material Diffuse", &material.diffuse[0], 0.0f, 1.0f);
        ImGui::SliderFloat3("Material Specular", &material.specular[0], 0.0f, 1.0f);
        ImGui::End();

        ImGui::Begin("Light");
        if (ImGui::Button("Add Point Light")) {
            lightManager.AddPointLight(new PointLight());
            shader.UseProgram();
            shader.SetInt("pointLightNumber", lightManager.GetPointLightCount());
        }
        if (ImGui::Button("SpotLight Switch")) {
            spotLightSwitch = !spotLightSwitch;
        }
        ImGui::SliderFloat("Light Size", &size, 0.1f, 1.0f);
        for (int i = 0; i < lightManager.GetPointLightCount(); i++) {
            char* name = new char[10];
            sprintf(name, "pointLight[%d]", i);
            ImGui::SliderFloat3(name, &(*lightManager.GetPointLight(i)).position[0], -15.0f, 10.0f);
        }
        ImGui::End();

        ImGui::Begin("Test");
        if (ImGui::Button("isStencilTest")) {
            isStencilTest = !isStencilTest;
        }

        ImGui::End();
        ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
         

        // glBindVertexArray(0); // no need to unbind it every time 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &lightVBO);

    
    shader.DeleteProgram();
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
     
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        right_mouse_pressed = true;
    }

    if (right_mouse_pressed) {
        camera.MouseMoveCameraView(xposIn, yposIn);
    }
    
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
        right_mouse_pressed = false;
        camera.SetFirstMouse(true);
    }
    
}

void sroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.MouseSrollCameraView(static_cast<float>(yoffset));
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
        if (nrChannels == 1)
            format = GL_RED;
        if (nrChannels == 3)
            format = GL_RGB;
        if (nrChannels == 4) {
            format = GL_RGBA;
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

unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
