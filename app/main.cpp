// @formatter:off
#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
// @formatter:on

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib>
#include <cmath>

#include "Shader.h"
#include "stb_image.h"
#include "Texture2D.h"

void frame_buffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "cetvrta-nedelja", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    Shader shader("resources/shaders/vertexShader.vs",
                  "resources/shaders/fragmentShader.fs");

    Shader axisShader("resources/shaders/axisVertexSh.vs",
                      "resources/shaders/axisFragmentSh.fs");


    // RAVAN
    float planeVertices[] = {
        -1.0f, -1.0f, 0.0f, // 0 levo dole
        1.0f, -1.0f, 0.0f, // 1 desno dole
        1.0f, 1.0f, 0.0f, // 2 desno gore
        -1.0f, 1.0f, 0.0f // 3 levo gore
    };
    unsigned int planeIndices[] = {
        0, 1, 2,
        0, 2, 3
    };

    // KVADRANTI
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0, 0.0, // 0 levo dole
        0.5f, -0.5f, 0.0f, 1.0, 0.0, // 1 desno dole
        0.5f, 0.5f, 0.0f, 1.0, 1.0, // 2 desno gore
        -0.5f, 0.5f, 0.0f, 0.0, 1.0 // 3 levo gore
    };

    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    // OSE
    float axisVertices[] = {
        // X-osa
        -1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        // Y-osa
        0.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        // Z-osa
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, 1.0f
    };

    int locationID = glGetUniformLocation(shader.ID(), "model");

    unsigned int VBO, VAO, EBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // OSE!!!
    unsigned int axisVAO, axisVBO;
    glGenVertexArrays(1, &axisVAO);
    glGenBuffers(1, &axisVBO);

    glBindVertexArray(axisVAO);
    glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    // KRAJ OSA :3

    // RAVAN!!!
    unsigned int planeVAO, planeVBO, planeEBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glGenBuffers(1, &planeEBO);

    glBindVertexArray(planeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeIndices), planeIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    // KRAJ RAVNI (dokle vise...)

    Texture2D textures[4] = {
        Texture2D("resources/textures/wood.jpg", GL_LINEAR, GL_REPEAT),
        Texture2D("resources/textures/soil.jpg", GL_LINEAR, GL_REPEAT),
        Texture2D("resources/textures/metal.jpg", GL_LINEAR, GL_REPEAT),
        Texture2D("resources/textures/stone.jpg", GL_LINEAR, GL_REPEAT)
    };
    shader.use();
    shader.setUniform1i("t0", 0);

    axisShader.use();
    int colorLoc = glGetUniformLocation(axisShader.ID(), "axisColor");
    int axisModelLoc = glGetUniformLocation(axisShader.ID(), "model");

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        processInput(window);
        glClearColor(0.1137, 0.2000, 0.3294, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glm::mat4 identity = glm::mat4(1.0f);

        // crtanje ravni
        axisShader.use();
        glUniformMatrix4fv(axisModelLoc, 1, GL_FALSE, glm::value_ptr(identity));
        glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 0.15f);
        glBindVertexArray(planeVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

        shader.use();
        glBindVertexArray(VAO);

        // kvadrant 1
        textures[0].active(GL_TEXTURE0);
        glm::mat4 quad1 = glm::mat4(1.0f);
        quad1 = glm::translate(quad1, glm::vec3(0.5f, 0.5f, 0.0f));
        glUniformMatrix4fv(locationID, 1, GL_FALSE, glm::value_ptr(quad1));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

        // kvadrant 2
        textures[1].active(GL_TEXTURE0);
        glm::mat4 quad2 = glm::mat4(1.0f);
        quad2 = glm::translate(quad2, glm::vec3(-0.5f, 0.5f, 0.0f));
        glUniformMatrix4fv(locationID, 1, GL_FALSE, glm::value_ptr(quad2));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

        // kvadrant 3
        textures[2].active(GL_TEXTURE0);
        glm::mat4 quad3 = glm::mat4(1.0f);
        quad3 = glm::translate(quad3, glm::vec3(-0.5f, -0.5f, 0.0f));
        glUniformMatrix4fv(locationID, 1, GL_FALSE, glm::value_ptr(quad3));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

        // kvadrant 4
        textures[3].active(GL_TEXTURE0);
        glm::mat4 quad4 = glm::mat4(1.0f);
        quad4 = glm::translate(quad4, glm::vec3(0.5f, -0.5f, 0.0f));
        glUniformMatrix4fv(locationID, 1, GL_FALSE, glm::value_ptr(quad4));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

        glBindVertexArray(0);


        // Crtaǌe osa
        axisShader.use();
        glBindVertexArray(axisVAO);

        int axisModelLoc = glGetUniformLocation(axisShader.ID(), "model");
        glUniformMatrix4fv(axisModelLoc, 1, GL_FALSE, glm::value_ptr(identity));

        glLineWidth(2.0f);

        glUniform3f(glGetUniformLocation(axisShader.ID(), "axisColor"), 1.0f, 0.0f, 0.0f);
        glDrawArrays(GL_LINES, 0, 2);

        glUniform3f(glGetUniformLocation(axisShader.ID(), "axisColor"), 0.0f, 1.0f, 0.0f);
        glDrawArrays(GL_LINES, 2, 2);

        glUniform3f(glGetUniformLocation(axisShader.ID(), "axisColor"), 0.0f, 0.0f, 1.0f);
        glDrawArrays(GL_LINES, 4, 2);

        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    shader.deleteProgram();
    glfwTerminate();
    return 0;
}

void frame_buffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    bool leftShift = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
    bool rightShift = (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
}
