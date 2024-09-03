#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"
#include"Shaders/Shader.h"
#include"Camera.h"

#include"glm/vec3.hpp"
#include"Data.h"

#include"stb_image.h"
#include <iostream>



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void sroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void load_image(const char* imageFile);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 1080;

float currentTime = 0;
float lastTime = 0;
bool firstMouse = true;
Camera camera;

bool right_mouse_pressed = false;

glm::vec3 lightPos(1.2f, 0.0f, 2.0f);

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
    // 开启深度缓冲
    glEnable(GL_DEPTH_TEST);


    Shader shader("Shaders/shaderSource/Vertex.shader", "Shaders/shaderSource/Fragment.Shader");
    Shader lightShader("Shaders/shaderSource/LightVertexShader.shader", "Shaders/shaderSource/LightFragmentShader.Shader");

	// 顶点缓冲对象(Vertex Buffer Objects, VBO) 管理顶点数组 (顶点数组对象(Vertex Array Objects, VAO))
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(VAO);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 光源
    unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
	// VBO 存放箱子的位置数据
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


	// ImGui 初始化
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

    float size = 1.0f;

    glm::vec3 objectColor = glm::vec3(1.0f, 0.5f, 0.31f);

    Material material(glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(0.5f, 0.5f, 0.5f), 32.0f);
    Light light(glm::vec3(0.2f), glm::vec3(0.5f), glm::vec3(1.0));
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
        // 清楚深度缓冲
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        camera.KeyboardMoveCamera(window, deltaTime);
        
        shader.UseProgram();
        shader.SetVec3f("objectColor", objectColor);

        glm::mat4 model(1.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.GetFOV()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		shader.SetMat4("model", model);
		shader.SetMat4("view", view);
		shader.SetMat4("projection", projection);
        shader.SetVec3f("lightPos", lightPos);

        // 法线矩阵
        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        shader.SetMat3("normalMatrix", normalMatrix);

        // 观察位置
        shader.SetVec3f("viewPos", camera.GetPos());

        // 物体材质
        shader.SetMaterial("material", material);

        // 光源
        shader.SetLight("light", light);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

       
        lightShader.UseProgram();
        lightShader.SetMat4("view", view);
        lightShader.SetMat4("projection", projection);
        lightShader.SetFloat("size", size);

        model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		lightShader.SetMat4("model", model);
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);


        float FOV = camera.GetFOV();

        // ImGui Optional
        ImGui::Begin("My name is window, ImGui window!");
        ImGui::Text("heelo the adventurer!");
        ImGui::SliderFloat("Light Size", &size, 0.1f, 1.0f);
        ImGui::SliderFloat3("Object Color", &objectColor[0], 0.0f, 1.0f);
        ImGui::End();

        ImGui::Begin("Material");
        ImGui::SliderFloat3("Material Ambient", &material.ambient[0], 0.0f, 1.0f);
        ImGui::SliderFloat3("Material Diffuse", &material.diffuse[0], 0.0f, 1.0f);
        ImGui::SliderFloat3("Material Specular", &material.specular[0], 0.0f, 1.0f);
        ImGui::End();

        ImGui::Begin("Light");
        ImGui::SliderFloat3("Light Ambient", &light.ambient[0], 0.0f, 1.0f);
        ImGui::SliderFloat3("Light Diffuse", &light.diffuse[0], 0.0f, 1.0f);
        ImGui::SliderFloat3("Light Specular", &light.specular[0], 0.0f, 1.0f);
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

void load_image(const char* imageFile) {
    // 加载并生成纹理
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
}