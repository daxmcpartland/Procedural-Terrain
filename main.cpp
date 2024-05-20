#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "src/Noise.h"
#include "src/Shader.h"
#include "src/Camera.h"
#define STB_IMAGE_IMPLEMENTATION
#include "src/stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
unsigned int loadCubemap(std::vector<std::string> faces);
unsigned int loadTexture(char const* path);

const unsigned int SCR_WIDTH = 1600, SCR_HEIGHT = 1300;

// camera 
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
    // create window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Procedural Terrian - OpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // intialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // change window size when updated
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  
    // update camera when mouse moved
    glfwSetCursorPosCallback(window, mouse_callback);
    // disable cursor 
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);


    // create the vertex and fragment shader
    Shader shader("shaders/VertexShader.glsl", "shaders/FragmentShader.glsl");
    Shader skyboxShader("shaders/SkyBoxVertex.glsl", "shaders/SkyBoxFragment.glsl");

    const int terrainGrid = 50;
    std::vector<float> vertices;
    float scale = 0.40f;
    Noise perlin;
    // create the height map from perlin noise
    std::vector<std::vector<float>> heightMap(terrainGrid, std::vector<float>(terrainGrid));
    for (int i = 0; i < terrainGrid; ++i) {
        for (int j = 0; j < terrainGrid; ++j) {
            heightMap[i][j] = perlin.perlin_noise(i / scale, j / scale, terrainGrid);

            if (heightMap[i][j] < -1.0)
                heightMap[i][j] = -1.0;
        }
    }

    // to make the textures not look stupid this will split up the grid into 16 sections
    // therefore each texture gets split up into 16 blocks
    float topLeftX = 0;
    float topLeftZ = 0;

    int sections = 4;
    int sectionSize = terrainGrid / sections;
    for (int s = 0; s < sections; ++s) {
        for (int t = 0; t < sections; ++t) {
            // calculate the starting indices for this section
            int startX = s * sectionSize;
            int startZ = t * sectionSize;

            for (int i = startX; i < startX + sectionSize; ++i) {
                for (int j = startZ; j < startZ + sectionSize; ++j) {
                    // calculate texture coordinates relative to this section
                    float x1 = (float)(i - startX) / (sectionSize - 1);
                    float x2 = (float)(i - startX + 1) / (sectionSize - 1);
                    float y1 = (float)(j - startZ) / (sectionSize - 1);
                    float y2 = (float)(j - startZ + 1) / (sectionSize - 1);

                    // first triangle
                    vertices.push_back(topLeftX + i);
                    vertices.push_back(heightMap[i][j]);
                    vertices.push_back(topLeftZ + j);
                    vertices.push_back(x1); // tex coordinates top left
                    vertices.push_back(y1);

                    vertices.push_back(topLeftX + i + 1);
                    vertices.push_back(heightMap[i + 1][j + 1]);
                    vertices.push_back(topLeftZ + j + 1);
                    vertices.push_back(x2); // tex coordinates bottom right
                    vertices.push_back(y2);

                    vertices.push_back(topLeftX + i);
                    vertices.push_back(heightMap[i][j + 1]);
                    vertices.push_back(topLeftZ + j + 1);
                    vertices.push_back(x1); // tex coordinates bottom left
                    vertices.push_back(y2);

                    // second triangle
                    vertices.push_back(topLeftX + i + 1);
                    vertices.push_back(heightMap[i + 1][j + 1]);
                    vertices.push_back(topLeftZ + j + 1);
                    vertices.push_back(x2); // tex coordinates bottom right
                    vertices.push_back(y2);

                    vertices.push_back(topLeftX + i);
                    vertices.push_back(heightMap[i][j]);
                    vertices.push_back(topLeftZ + j);
                    vertices.push_back(x1); // tex coordinates top left
                    vertices.push_back(y1);

                    vertices.push_back(topLeftX + i + 1);
                    vertices.push_back(heightMap[i + 1][j]);
                    vertices.push_back(topLeftZ + j);
                    vertices.push_back(x2); // tex coordinates top right
                    vertices.push_back(y1);
                }
            }
        }
    }

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    std::vector<std::string> faces
    {
        "textures/skybox/right.jpg",
        "textures/skybox/left.jpg",
        "textures/skybox/top.jpg",
        "textures/skybox/bottom.jpg",
        "textures/skybox/front.jpg",
        "textures/skybox/back.jpg"
    };
    unsigned int cubemapTexture = loadCubemap(faces);
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    unsigned int water = loadTexture("textures/water.jpg");
    unsigned int sand = loadTexture("textures/smooth_sand.jpg");
    unsigned int soil = loadTexture("textures/soil.jpg");
    unsigned int grass = loadTexture("textures/grass.jpg");
    unsigned int rock = loadTexture("textures/rock.jpg");
    unsigned int snow = loadTexture("textures/rocky.jpg");

    // must use shader first before being able to set stuff
    shader.use();
    shader.setInt("water", 0);
    shader.setInt("sand", 1);
    shader.setInt("soil", 2);
    shader.setInt("grass", 3);
    shader.setInt("rock", 4);
    shader.setInt("snow", 5);

    std::cout << "Rendering..." << std::endl;
    // the render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // don't forget to enable shader before setting uniforms
        shader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, water);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, sand);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, soil);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, grass);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, rock);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, snow);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size()/3);

        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    // camera controls
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
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
    camera.ProcessMouseMovement(xoffset, yoffset);
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    stbi_set_flip_vertically_on_load(false);
    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        const char* c_str = faces[i].c_str();
        unsigned char* data = stbi_load(c_str, &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
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



