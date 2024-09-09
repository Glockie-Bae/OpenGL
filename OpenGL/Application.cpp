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

#define STB_IMAGE_IMPLEMENTATION
#include"stb/stb_image.h"



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void sroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int load_image(const char* imageFil, Shader shader);

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
    // ������Ȼ���
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);




    Shader shader("Shaders/shaderSource/Vertex.shader", "Shaders/shaderSource/Fragment.Shader");
    Shader lightShader("Shaders/shaderSource/LightVertexShader.shader", "Shaders/shaderSource/LightFragmentShader.Shader");
    Shader shaderSingleColor("Shaders/shaderSource/DepthTestVS.shader", "Shaders/shaderSource/DepthTestFS.shader");

	// ���㻺�����(Vertex Buffer Objects, VBO) ���������� (�����������(Vertex Array Objects, VAO))
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(VAO);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // ��Դ
    unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
	// VBO ������ӵ�λ������
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    shader.UseProgram();
    unsigned int texture1 = load_image("res/marble.jpg", shader);
	shader.SetInt("texture_1", 0);

    unsigned int texture2 = load_image("res/metal.jpg", shader);
    shader.SetInt("texture_2", 1);

    unsigned int texture3 = load_image("res/metal.jpg", shader);
    shader.SetInt("texture_3", 2);

 

	// ImGui ��ʼ��
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

    float size = 1.0f;

    glm::vec3 objectColor = glm::vec3(1.0f, 0.5f, 0.31f);

    //glm::vec3(1.2f, 0.58f, 2.0f)
    Material material(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), 128.0f);


    LightManager lightManager;
    lightManager.AddDirLight(new DirLight(glm::vec3(0.2f), glm::vec3(0.5f), glm::vec3(0.5f), glm::vec3(0.0f, 0.0f, -2.0f)));

    lightManager.AddSpotLight(new SpotLight(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(1.0f), camera.GetPos(), camera.GetFront(), 12.5f, 15.0f, 1.0f, 0.09f, 0.032f));
    for (int i = 0; i < 1; i++) {
        lightManager.AddPointLight(new PointLight(glm::vec3(0.2f), glm::vec3(1.0f), glm::vec3(1.0f), pointLightPositions[i], 1.0f, 0.09f, 0.032f));
    }

    bool spotLightSwitch = true;

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
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        // �����Ȼ���
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
     

        currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        camera.KeyboardMoveCamera(window, deltaTime);

        shaderSingleColor.UseProgram();
        glm::mat4 model(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.GetFOV()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shaderSingleColor.SetMat4("view", view);
        shaderSingleColor.SetMat4("projection", projection);

        

        // model shader
        shader.UseProgram();
        shader.SetMaterial("material", material);
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.GetFOV()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shader.SetMat4("view", view);
        shader.SetMat4("projection", projection);

        // �Ȼ���ƽ��
        glStencilMask(0x00);
        // floor
        glBindVertexArray(planeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture2);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        shader.SetMat4("model", glm::mat4(1.0f));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);



        // 1st. render pass, draw objects as normal, writing to the stencil buffer
        // --------------------------------------------------------------------
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        // cubes
        glBindVertexArray(VAO);
        // model shader texture setting
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture1);

		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        shader.SetMat4("model", model);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        shader.SetMat4("model", model);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 2nd. render pass: now draw slightly scaled versions of the objects, this time disabling stencil writing.
         // Because the stencil buffer is now filled with several 1s. The parts of the buffer that are 1 are not drawn, thus only drawing 
         // the objects' size differences, making it look like borders.
         // -----------------------------------------------------------------------------------------------------------------------------
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
        shaderSingleColor.UseProgram();
        float scale = 1.1f;
        // cubes
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture1);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        model = glm::scale(model, glm::vec3(scale, scale, scale));
        shaderSingleColor.SetMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(scale, scale, scale));
        shaderSingleColor.SetMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glEnable(GL_DEPTH_TEST);
 


        // model shader light setting
        shader.SetDirLight("dirLight", *lightManager.GetDirLight(0));
		shader.SetInt("pointLightNumber", lightManager.GetPointLightCount());
        for (int i = 0; i < lightManager.GetPointLightCount(); i++) {
            char* name = new char[10];
            sprintf(name, "pointLight[%d]", i);
            shader.SetPointLight(name, *lightManager.GetPointLight(i));
        }
        shader.SetSpotLight("spotLight", *lightManager.GetSpotLight(0));
        shader.SetVec3f("viewPos", camera.GetPos());


        

        



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
        ImGui::Begin("My name is window, ImGui window!");
        ImGui::Text("heelo the adventurer!");
        ImGui::SliderFloat("Light Size", &size, 0.1f, 1.0f);
        ImGui::SliderFloat3("DirLight Direction", &(*lightManager.GetDirLight(0)).direction[0], -10.f, 10.0f);
        ImGui::SliderFloat3("Object Color", &objectColor[0], 0.0f, 1.0f);
        ImGui::SliderFloat3("Object1 Position", &cubePositions[0][0], -10.0f, 10.0f);
        ImGui::End();

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
        for (int i = 0; i < lightManager.GetPointLightCount(); i++) {
            char* name = new char[10];
            sprintf(name, "pointLight[%d]", i);
            ImGui::SliderFloat3(name, &(*lightManager.GetPointLight(i)).position[0], -15.0f, 10.0f);
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
    glDeleteBuffers(1, &VBO);
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

unsigned int load_image(const char* imageFile, Shader shader) {
    // ���ز���������
    unsigned int texTureID;
    shader.BindTexture(texTureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(imageFile, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    return texTureID;
}