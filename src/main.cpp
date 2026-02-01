#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "camera.h"
#include "shader.h"
#include "stb_image.h"
#include "chunk.h"

namespace Settings
{
    constexpr int SCREEN_WIDTH { 800 };
    constexpr int SCREEN_HEIGHT { 600 };
    const char* WINDOW_NAME { "Minecraft" };
}

bool firstMouse = true;

float yaw = -90.0f;
float pitch = 0.0f;

float lastX = Settings::SCREEN_WIDTH / 2.0f;
float lastY = Settings::SCREEN_HEIGHT / 2.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera camera { Settings::SCREEN_WIDTH, Settings::SCREEN_HEIGHT };

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* const window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void loadTexture(GLuint* texture , const char filePath[]);
void initBuffers(GLuint* VBO, GLuint* VAO, float* vertices, size_t verticesCount);

int main()
{
    // setup glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // setup window
    GLFWwindow* window = glfwCreateWindow(Settings::SCREEN_WIDTH, Settings::SCREEN_HEIGHT, Settings::WINDOW_NAME, NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1; 
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    // setup glad
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    float vertices[] = {
        // back face
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,

        // front face
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        // left face
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,

        // right face
        0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f, 1.0f,

        // bottom face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        // top face
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    // init buffers
    GLuint VBO {};
    GLuint VAO {};
    initBuffers(&VBO, &VAO, vertices, sizeof(vertices));

    // load texture atlas
    GLuint textureAtlas {};
    loadTexture(&textureAtlas, "C:\\Users\\Andrew\\projects\\opengl\\src\\textures.png");

    // init shader
    Shader shader { "C:\\Users\\Andrew\\projects\\opengl\\src\\shaders\\basic.vert", "C:\\Users\\Andrew\\projects\\opengl\\src\\shaders\\basic.frag" };
    shader.use();
    shader.setInt("texture", 0);

    // settings
    glEnable(GL_DEPTH_TEST);

    // make a chunk
    Chunk chunk {};

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        glm::mat4 projection = projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        GLint projectionLoc = glGetUniformLocation(shader.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 view = camera.getViewMatrix();
        GLint viewLoc = glGetUniformLocation(shader.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureAtlas);

        glBindVertexArray(VAO);

        for (const Block& block : chunk.blocks())
        {  
            if (!block.isRendered())
            {
                continue;
            }

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(block.x, block.y, block.z));

            GLint modelLoc = glGetUniformLocation(shader.ID, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            glm::vec2 tileCoord { block.getAtlasCoord() };

            shader.setFloat("xTile", tileCoord.x);
            shader.setFloat("yTile", tileCoord.y);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void initBuffers(GLuint* VBO, GLuint* VAO, float* vertices, size_t verticesCount)
{
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);

    glBindVertexArray(*VAO);

    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, verticesCount, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void loadTexture(GLuint* texture , const char filePath[])
{
    int width {};
    int height {};
    int nrChannels {};
    stbi_set_flip_vertically_on_load(true);
    stbi_uc* data { stbi_load(filePath, &width, &height, &nrChannels, 0) };

    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture\n";
    }

    stbi_image_free(data);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* const window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.processKeyInput(CameraDirection::FORWARD, deltaTime);
    }

    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.processKeyInput(CameraDirection::BACK, deltaTime);
    }

    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.processKeyInput(CameraDirection::RIGHT, deltaTime);
    }

    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.processKeyInput(CameraDirection::LEFT, deltaTime);
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.processMouseInput(xoffset, yoffset);
}
