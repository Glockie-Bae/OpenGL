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
#include"Render/Renderer.h"

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

unsigned int indexCount;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_SAMPLES, 4);
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
    glDepthFunc(GL_LEQUAL); // set depth function to less than AND equal for skybox depth trick.
    
    Shader shader("Shaders/shaderSource/Vertex.shader", "Shaders/shaderSource/Fragment.shader");
    Shader lightShader("Shaders/shaderSource/LightVertexShader.shader", "Shaders/shaderSource/LightFragmentShader.Shader");
    Shader skyBoxShader("Shaders/shaderSource/SkyBoxVertex.shader", "Shaders/shaderSource/SkyBoxFragment.shader");
    Shader planeShader("Shaders/shaderSource/DepthTestVS.shader", "Shaders/shaderSource/DepthTestFS.shader");
    Shader hdrShader("Shaders/shaderSource/hdrVS.shader", "Shaders/shaderSource/hdrFS.shader");
    Shader PBRShader("Shaders/shaderSource/PBRVS.shader", "Shaders/shaderSource/PBRFS.shader");
    Shader hdrCubeMapShader("Shaders/shaderSource/hdrCubeMapVS.shader", "Shaders/shaderSource/hdrCubeMapFS.shader");
	Shader backgroundShader("Shaders/shaderSource/backgroundVS.shader", "Shaders/shaderSource/backgroundFS.shader");

	// set up vertex data and configure vertex attributes
    Renderer renderer;
    renderer.CreateBufferObject("quad");
    renderer.CreateBufferObject("plane");
    renderer.CreateBufferObject("skybox");
    renderer.CreateBufferObject("pointlight");
    renderer.CreateBufferObject("sphere");
	renderer.CreateBufferObject("framebuffer");
    renderer.CreateBufferObject("cube");

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

    backgroundShader.UseProgram();
    backgroundShader.SetInt("environmentMap", 0);

	unsigned int hdrTexture = load_hdr_image("res/hdr/newport_loft.hdr");



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
    Material material(glm::vec3(0.5f, 0.0f, 0.0f), 0.5f, 0.5f, 1.0f);
    material.LoadTextureMap(std::string("res/rust"));

    PBRShader.UseProgram();
    PBRShader.SetInt("albedoMap", 0);
    PBRShader.SetInt("normalMap", 1);
    PBRShader.SetInt("metallicMap", 2);
    PBRShader.SetInt("roughnessMap", 3);
    PBRShader.SetInt("aoMap", 4);



    LightManager lightManager;
    lightManager.AddDirLight(new DirLight(glm::vec3(0.2f), glm::vec3(0.5f), glm::vec3(0.5f), glm::vec3(0.0f, 0.0f, -2.0f)));
    lightManager.AddSpotLight(new SpotLight(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(1.0f), camera.GetPos(), camera.GetFront(), 12.5f, 15.0f, 1.0f, 0.09f, 0.032f));
    for (int i = 0; i < 1; i++) {
        lightManager.AddPointLight(new PointLight(glm::vec3(0.2f), glm::vec3(1.0f), glm::vec3(1.0f), pointLightPositions[i], 1.0f, 0.09f, 0.032f));
        lightManager.AddPointLight(new PointLight(glm::vec3(0.2f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(-2.0f, 4.0f, -1.0f), 1.0f, 0.09f, 0.032f));
    }

    bool spotLightSwitch = true;
    bool isStencilTest = false;
    bool glass = true;
    bool Isblinn = true;
    bool IsNormalTexture = true;
    bool IsHDR = true;
    bool IsTexture = true;
    float exposure = 1.0f;



    // pbr: setup framebuffer
// ----------------------
    unsigned int captureFBO;
    unsigned int captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    // pbr: setup cubemap to render to and attach to framebuffer
    // ---------------------------------------------------------
    unsigned int envCubemap;
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
    // ----------------------------------------------------------------------------------------------
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    hdrCubeMapShader.UseProgram();
    hdrCubeMapShader.SetInt("equirectangularMap", 5);
    hdrCubeMapShader.SetMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        hdrCubeMapShader.SetMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderer.RenderCube("cube");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // initialize static shader uniforms before rendering
    // --------------------------------------------------
    glm::mat4 model(1.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.GetFOV()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
    PBRShader.UseProgram();
    PBRShader.SetMat4("projection", projection);
    backgroundShader.UseProgram();
    backgroundShader.SetMat4("projection", projection);

    // then before rendering, configure the viewport to the original framebuffer's screen dimensions
    int scrWidth, scrHeight;
    glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
    glViewport(0, 0, scrWidth, scrHeight);


    float light = 1.0f;
    int nrRows = 7;
    int nrColumns = 7;
    float spacing = 2.5;
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
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // 清楚深度缓冲
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        camera.KeyboardMoveCamera(window, deltaTime);


        PBRShader.UseProgram();
        PBRShader.SetMaterial("material", material);
        view = camera.GetViewMatrix();
        PBRShader.SetMat4("view", view);
        PBRShader.SetVec3f("camPos", camera.GetPos());
        PBRShader.SetMat4("projection", projection);
        PBRShader.SetBool("IsTexture", IsTexture);


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material.textureMap.albedoMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, material.textureMap.normalMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, material.textureMap.metallicMap);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, material.textureMap.roughnessMap);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, material.textureMap.aoMap);

       hdrCubeMapShader.UseProgram();
	   hdrCubeMapShader.SetMat4("view", view);
       hdrCubeMapShader.SetMat4("projection", projection);
       glActiveTexture(GL_TEXTURE5);
       glBindTexture(GL_TEXTURE_2D, hdrTexture);
       renderer.RenderCube("cube");

        PBRShader.UseProgram();
        // render rows*column number of spheres with varying metallic/roughness values scaled by rows and columns respectively
        model = glm::mat4(1.0f);
        for (int row = 0; row < nrRows; ++row)
        {
            PBRShader.SetFloat("material.metallic", (float)row / (float)nrRows);
            for (int col = 0; col < nrColumns; ++col)
            {
                // we clamp the roughness to 0.05 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
                // on direct lighting.
                PBRShader.SetFloat("material.roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(
                    (col - (nrColumns / 2)) * spacing,
                    (row - (nrRows / 2)) * spacing,
                    0.0f
                ));
                PBRShader.SetMat4("model", model);
                PBRShader.SetMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
                renderer.RenderSphere("sphere");
            }
        }

        // render light source (simply re-render sphere at light positions)
        // this looks a bit off as we use the same shader, but it'll make their positions obvious and 
        // keeps the codeprint small.
        for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
        {
            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            newPos = lightPositions[i];
            PBRShader.UseProgram();
            PBRShader.SetVec3f("lightPositions[" + std::to_string(i) + "]", newPos);
            PBRShader.SetVec3f("lightColors[" + std::to_string(i) + "]", lightColors[i]);

            model = glm::mat4(1.0f);
            model = glm::translate(model, newPos);
            model = glm::scale(model, glm::vec3(1.0f));

            PBRShader.SetMat4("model", model);
            PBRShader.SetMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
            renderer.RenderSphere("sphere");
        }

        // light shader
        // setting light cube
        lightShader.UseProgram();
        lightShader.SetMat4("view", view);
        lightShader.SetMat4("projection", projection);
        lightShader.SetFloat("size", size);
    
        for (int i = 0; i < lightManager.GetPointLightCount(); i++) {
            glm::mat4 lightModel = glm::mat4(1.0f);
            lightModel = glm::translate(lightModel, (*lightManager.GetPointLight(i)).position);
            lightModel = glm::scale(lightModel, glm::vec3(0.2f)); // a smaller cube
            lightShader.SetMat4("model", lightModel);
            renderer.Render("pointlight", vertices, sizeof(vertices), 3, 3, true);
        }

        //// render skybox (render as last to prevent overdraw)
        backgroundShader.UseProgram();
        backgroundShader.SetMat4("view", view);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        renderer.RenderCube("cube");


        // ImGui Optional
        ImGui::Begin("Light");
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
        if (ImGui::Button("IsBlinn")) {
            Isblinn = !Isblinn;
        }
        if (ImGui::Button("IsNormalTexture")) {
            IsNormalTexture = !IsNormalTexture;
        }
        if (ImGui::Button("IsHDR")) {
            IsHDR = !IsHDR;
        }
        if (ImGui::Button("IsTexture")) {
            IsTexture = !IsTexture;
        }
		ImGui::DragFloat3("albedo", &material.albedo[0], 0.01f, 0.0f, 1.0f);
        ImGui::SliderFloat("exposure", &exposure, -1.0f, 2.0f);
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
    // -----------------------------------------------------------------------
    
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
