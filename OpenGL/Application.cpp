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

#include"PBR/pbrBuffer.h"
#include"WindowManager.h"
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
void renderQuad();

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
    Shader irradianceShader("Shaders/shaderSource/hdrCubeMapVS.shader", "Shaders/shaderSource/irradianceFS.shader");
	Shader prefilterShader("Shaders/shaderSource/hdrCubeMapVS.shader", "Shaders/shaderSource/prefilterFS.shader");
    Shader brdfShader("Shaders/shaderSource/brdfVS.shader", "Shaders/shaderSource/brdfFS.shader");

    WindowManager windowManager;
	// set up vertex data and configure vertex attributes
    windowManager.AddRenderType("quad");
	windowManager.AddRenderType("plane");
	windowManager.AddRenderType("skybox");
	windowManager.AddRenderType("pointlight");
	windowManager.AddRenderType("sphere");
	windowManager.AddRenderType("framebuffer");
	windowManager.AddRenderType("cube");


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

	//Model ourModel("./res/nanosuit/nanosuit.obj");
	

    float size = 0.1f;
    //glm::vec3(1.2f, 0.58f, 2.0f)
    Material gold;
    gold.LoadTextureMap(std::string("res/pbr/gold"));

	Material grassMaterial;
    grassMaterial.LoadTextureMap(std::string("res/pbr/grass"));

    Material plasticMaterial;
    plasticMaterial.LoadTextureMap(std::string("res/pbr/plastic"));

    Material rusted_iron;
    rusted_iron.LoadTextureMap(std::string("res/pbr/rusted_iron"));




    PBRShader.UseProgram();
    PBRShader.SetInt("albedoMap", 0);
    PBRShader.SetInt("normalMap", 1);
    PBRShader.SetInt("metallicMap", 2);
    PBRShader.SetInt("roughnessMap", 3);
    PBRShader.SetInt("aoMap", 4);
    PBRShader.SetInt("irradianceMap", 5);
    PBRShader.SetInt("prefilterMap", 6);
    PBRShader.SetInt("brdfLUT", 7);

    backgroundShader.UseProgram();
    backgroundShader.SetInt("environmentMap", 0);



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
    float cubeSize = 1.0f;

    PBR_BUFFER pbrBuffer(512);
    unsigned int captureFBO = *(pbrBuffer.GetCaptureFBO());
    unsigned int captureRBO = *(pbrBuffer.GetCaptureRBO());

    // pbr: setup cubemap to render to and attach to framebuffer
    // ---------------------------------------------------------
    unsigned int envCubemap;
    //pbrBuffer.BindEvironmentMapTexture(envCubemap, 512);
    // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
    // ----------------------------------------------------------------------------------------------
    pbrBuffer.BindEvironmentMapTexture(envCubemap, 512);

    hdrCubeMapShader.UseProgram();
    hdrCubeMapShader.SetInt("equirectangularMap", 0);
    hdrCubeMapShader.SetMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    //windowManager.RenderEvironmentMapTexture(envCubemap, hdrCubeMapShader, 512);

    pbrBuffer.BindFrameBuffer(512);

    glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        hdrCubeMapShader.SetMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        windowManager.m_renderer->RenderCube("cube");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
    // --------------------------------------------------------------------------------
    unsigned int irradianceMap;
    // pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
    pbrBuffer.BindIrradianceTexture(irradianceMap, 32);
   // -----------------------------------------------------------------------------
    irradianceShader.UseProgram();
    irradianceShader.SetInt("environmentMap", 0);
    irradianceShader.SetMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);


    //windowManager.RenderIrradianceTexture(&irradianceMap, &irradianceShader, 32);
    pbrBuffer.BindFrameBuffer(32);
    for (unsigned int i = 0; i < 6; ++i)
    {
        irradianceShader.SetMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        windowManager.RenderCube("cube");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    unsigned int prefilterMap;

    // pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
    // ----------------------------------------------------------------------------------------------------
    prefilterShader.UseProgram();
    prefilterShader.SetInt("environmentMap", 0);
    prefilterShader.SetMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    pbrBuffer.BindPrefilterTexture(prefilterMap, 128);


    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // reisze framebuffer according to mip-level size.
        // mipmap -> 128 -> 64 -> 32 -> 16 -> 8
        unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilterShader.SetFloat("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            prefilterShader.SetMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            windowManager.RenderCube("cube");
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::cout << std::pow(0.5, 2) << std::endl;
    // pbr: generate a 2D LUT from the BRDF equations used.
    // ----------------------------------------------------
    unsigned int brdfLUTTexture;
    pbrBuffer.BindBRDFTexture(brdfLUTTexture, 512);

    glViewport(0, 0, 512, 512);
    brdfShader.UseProgram();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderQuad();

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
    bool IsIrradianceMap = true;
    glm::vec3 albedo = glm::vec3(0.5f, 0.0f, 0.0f);
    float metallic = 0.0f;
    float roughness = 0.0f;

    // ImGui 初始化
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

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
        view = camera.GetViewMatrix();
        PBRShader.SetMat4("view", view);
        PBRShader.SetVec3f("camPos", camera.GetPos());
        PBRShader.SetMat4("projection", projection);
        PBRShader.SetBool("IsTexture", IsTexture);
        PBRShader.SetBool("IsIrradianceMap", IsIrradianceMap);

        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);



        PBRShader.UseProgram();
        // render rows*column number of spheres with varying metallic/roughness values scaled by rows and columns respectively
		PBRShader.SetFloat("material.metallic", metallic);
        PBRShader.SetFloat("material.roughness", roughness);
        PBRShader.SetVec3f("material.albedo", albedo);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-5.0f, 0.0f, 0.0f));
        PBRShader.SetMat4("model", model);
        PBRShader.SetMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
        PBRShader.BindMaterialTexture(gold);
        windowManager.RenderSphere("sphere");

        model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
        PBRShader.SetMat4("model", model);
        PBRShader.SetMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
        PBRShader.BindMaterialTexture(grassMaterial);
        windowManager.RenderSphere("sphere");

        model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
        PBRShader.SetMat4("model", model);
        PBRShader.SetMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
        PBRShader.BindMaterialTexture(plasticMaterial);
        windowManager.RenderSphere("sphere");

        model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
        PBRShader.SetMat4("model", model);
        PBRShader.SetMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
        PBRShader.BindMaterialTexture(rusted_iron);
        windowManager.RenderSphere("sphere");

            

      

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
            windowManager.RenderSphere("sphere");
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
            windowManager.m_renderer->Render("pointlight", vertices, sizeof(vertices), 3, 3, true);
            windowManager.RenderSphere("sphere");
        }



        //// render skybox (render as last to prevent overdraw)
        backgroundShader.UseProgram();
        backgroundShader.SetMat4("view", view);
        glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        windowManager.RenderCube("cube");


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
        ImGui::SliderFloat("metallic", &metallic, 0.0f, 1.0f);
        ImGui::SliderFloat("roughness", &roughness, 0.0f, 1.0f);
		ImGui::DragFloat3("albedo", &albedo[0], 0.01f, 0.0f, 1.0f);
        ImGui::Checkbox("IsTexture", &IsTexture);
        ImGui::Checkbox("IsIrradianceMap", &IsIrradianceMap);
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

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}