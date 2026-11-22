// @formatter:off
#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
// @formatter:on

#include <cstdlib>

void frame_buffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);


const char* vertexShaderSource = R"S(
#version 330 core
layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)S";

const char* fragmentShaderSource = R"S(
#version 330 core
out vec4 FragColor;

uniform vec4 u_Color;

void main()
{
    FragColor = u_Color;
}
)S";

int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

int triangleNum = 0;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "druga-nedelja", NULL, NULL);
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

    unsigned vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success = 0;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED" << infoLog << std::endl;
    }

    unsigned fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED" << infoLog << std::endl;
    }

    unsigned shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // @formatter:off
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // 0 levo dole
        0.5f, -0.5f, 0.0f,  // 1 desno dole
        0.5f, 0.5f, 0.0f,   // 2 desno gore
        -0.5f, 0.5f, 0.0f,  // 3 levo gore
        0.0f, 0.75f, 0.0f,  // 4 gore centar
        0.0f, -0.75f, 0.0f, // 5 dole centar
        0.75f, 0.0f, 0.0f,  // 6 desno centar
        -0.75f, 0.0f, 0.0f, // 7 levo centar
        -0.25f, 0.75f, 0.0f // 8
    };
    // @formatter:on

    unsigned int indices[] = {
        0, 1, 2, // trougao
        0, 2, 3, // cetvorougao
        2, 3, 4, // petougao
        0, 1, 5, // sestougao
        1, 2, 6, // sedmougao
        0, 3, 7, // osmougao
        3, 4, 8 // devetougao
    };

    float quadVertices[] = {
        -1.0f, 1.0f, 0.0f, // 0 levo gore
        0.0f, 1.0f, 0.0f, // 1 centar gore
        0.0f, 0.0f, 0.0f, // 2 centar centar
        -1.0f, 0.0f, 0.0f, // 3 levo centar
        1.0f, 1.0f, 0.0f, // 4 desno gore
        1.0f, 0.0f, 0.0f, // 5 desno centar
        -1.0f, -1.0f, 0.0f, // 6 levo dole
        0.0f, -1.0f, 0.0f, // 7 levo centar
        1.0f, -1.0f, 0.0f, // 8 desno dole


    };

    unsigned int quadIndices[] = {
        0, 1, 2,
        0, 2, 3,
        4, 1, 2,
        4, 5, 2,
        3, 6, 2,
        7, 6, 2,
        8, 5, 2,
        8, 7, 2,

    };

    unsigned int qVAO, qVBO, qEBO;

    glGenVertexArrays(1, &qVAO);
    glBindVertexArray(qVAO);

    glGenBuffers(1, &qVBO);
    glBindBuffer(GL_ARRAY_BUFFER, qVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &qEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, qEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned int VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    int colorLoc = glGetUniformLocation(shaderProgram, "u_Color");

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        processInput(window);
        glClearColor(0.1137, 0.2000, 0.3294, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glBindVertexArray(qVAO);
        glUniform4f(colorLoc, 1.0, 0.0f, 0.5f, 1.0f);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
        glUniform4f(colorLoc, 1.0, 0.0f, 1.0f, 1.0f);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(float) * 6));
        glUniform4f(colorLoc, 1.0, 0.5f, 1.0f, 1.0f);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(float) * 12));
        glUniform4f(colorLoc, 1.0, 0.75f, 1.0f, 1.0f);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(float) * 18));

        glUniform4f(colorLoc, 0.6902f, 0.6392f, 0.8314f, 1.0f);
        glBindVertexArray(VAO);

        if (triangleNum == 3)
        {
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)0);
        }
        else if (triangleNum == 4)
        {
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
        }
        else if (triangleNum == 5)
        {
            glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, (void*)0);
        }
        else if (triangleNum == 6)
        {
            glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, (void*)0);
        }
        else if (triangleNum == 7)
        {
            glDrawElements(GL_TRIANGLES, 15, GL_UNSIGNED_INT, (void*)0);
        }
        else if (triangleNum == 8)
        {
            glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, (void*)0);
        }
        else if (triangleNum == 9)
        {
            glDrawElements(GL_TRIANGLES, 21, GL_UNSIGNED_INT, (void*)0);
        }


        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

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

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
    {
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        {
            triangleNum = 3;
        }

        else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        {
            triangleNum = 4;
        }
        else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
        {
            triangleNum = 5;
        }

        else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
        {
            triangleNum = 6;
        }
        else if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
        {
            triangleNum = 7;
        }
        else if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
        {
            triangleNum = 8;
        }
        else if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
        {
            triangleNum = 9;
        }
    }
}
