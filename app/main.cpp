// @formatter:off
#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
// @formatter:on

#include <cstdlib>
#include <cmath>

#include "Shader.h"
#include "stb_image.h"
#include "Texture2D.h"

void frame_buffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, Texture2D textures[]);

int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;


// cuvam float za svaku rgba komponentu jer je realno mala (3-dim matrica)
float quadrantColors[2][2][4] = {
    {{1.0f, 1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 0.0f}}, // y=0
    {{1.0f, 1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 0.0f}} // y=1
};

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "treca-nedelja", NULL, NULL);
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

    // @formatter:off
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0, 0.0, // 0 levo dole
        0.5f, -0.5f, 0.0f, 1.0, 0.0,  // 1 desno dole
        0.5f, 0.5f, 0.0f, 1.0, 1.0, // 2 desno gore
        -0.5f, 0.5f, 0.0f, 0.0, 1.0 // 3 levo gore
    };
    // @formatter:on

    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3
    };

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

    Texture2D textures[4] = {
        Texture2D("resources/textures/wood.jpg", GL_LINEAR, GL_REPEAT),
        Texture2D("resources/textures/soil.jpg", GL_LINEAR, GL_REPEAT),
        Texture2D("resources/textures/metal.jpg", GL_LINEAR, GL_REPEAT),
        Texture2D("resources/textures/stone.jpg", GL_LINEAR, GL_REPEAT)
    };

    shader.use();
    shader.setUniform1i("t0", 0);

    int gridSizeX = 2;
    int gridSizeY = 2;

    while (!glfwWindowShouldClose(window))
    {
        shader.checkForChanges();
        glfwPollEvents();
        processInput(window, textures);
        glClearColor(0.1137, 0.2000, 0.3294, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        glBindVertexArray(VAO);

        for (int y = 0; y < gridSizeY; y++)
        {
            for (int x = 0; x < gridSizeX; x++)
            {
                float posX = x - 0.5f;
                float posY = y - 0.5f;

                shader.setUniform2f("u_Offset", posX, posY);
                shader.setUniform4f("u_Color",
                                    quadrantColors[x][y][0],
                                    quadrantColors[x][y][1],
                                    quadrantColors[x][y][2],
                                    quadrantColors[x][y][3]);

                // preslikavanje N^2 -> N
                int texIndex = (y * gridSizeX) + x;
                textures[texIndex].active(GL_TEXTURE0);

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
            }
        }
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

void processInput(GLFWwindow* window, Texture2D textures[])
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }


    bool leftShift = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
    bool rightShift = (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);

    if (leftShift || rightShift)
    {
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        {
            for (int i = 0; i < 4; i++)
            {
                textures[i].setFiltering(GL_LINEAR);
            }
        }
        if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
        {
            for (int i = 0; i < 4; i++)
            {
                textures[i].setFiltering(GL_NEAREST);
            }
        }
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            quadrantColors[0][1][0] = 1.0;
            quadrantColors[0][1][1] = 0.0;
            quadrantColors[0][1][2] = 0.0;
        }
        else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        {
            quadrantColors[0][1][0] = 0.0;
            quadrantColors[0][1][1] = 1.0;
            quadrantColors[0][1][2] = 0.0;
        }
        else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        {
            quadrantColors[0][1][0] = 0.0;
            quadrantColors[0][1][1] = 0.0;
            quadrantColors[0][1][2] = 1.0;
        }
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        {
            quadrantColors[0][1][3] = 1.0f;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            quadrantColors[0][0][0] = 1.0;
            quadrantColors[0][0][1] = 0.0;
            quadrantColors[0][0][2] = 0.0;
        }
        else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        {
            quadrantColors[0][0][0] = 0.0;
            quadrantColors[0][0][1] = 1.0;
            quadrantColors[0][0][2] = 0.0;
        }
        else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        {
            quadrantColors[0][0][0] = 0.0;
            quadrantColors[0][0][1] = 0.0;
            quadrantColors[0][0][2] = 1.0;
        }
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        {
            quadrantColors[0][0][3] = 1.0f;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            quadrantColors[1][0][0] = 1.0;
            quadrantColors[1][0][1] = 0.0;
            quadrantColors[1][0][2] = 0.0;
        }
        else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        {
            quadrantColors[1][0][0] = 0.0;
            quadrantColors[1][0][1] = 1.0;
            quadrantColors[1][0][2] = 0.0;
        }
        else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        {
            quadrantColors[1][0][0] = 0.0;
            quadrantColors[1][0][1] = 0.0;
            quadrantColors[1][0][2] = 1.0;
        }
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        {
            quadrantColors[1][0][3] = 1.0f;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            quadrantColors[1][1][0] = 1.0;
            quadrantColors[1][1][1] = 0.0;
            quadrantColors[1][1][2] = 0.0;
        }
        else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        {
            quadrantColors[1][1][0] = 0.0;
            quadrantColors[1][1][1] = 1.0;
            quadrantColors[1][1][2] = 0.0;
        }
        else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        {
            quadrantColors[1][1][0] = 0.0;
            quadrantColors[1][1][1] = 0.0;
            quadrantColors[1][1][2] = 1.0;
        }
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        {
            quadrantColors[1][1][3] = 1.0f;
        }
    }
}
