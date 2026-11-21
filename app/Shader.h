//
// Created by aleksndra on 11/19/26.
//

#ifndef PROJECT_SHADER_H
#define PROJECT_SHADER_H

#include <string>
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "Error.h"

std::string readFileContents(std::string path)
{
    std::ifstream in(path);
    std::stringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}


class Shader
{
    unsigned int m_Id;

public:
    Shader(std::string vertexShaderPath, std::string fragmentShaderPath)
    {
        // vertex shader
        std::string vsString = readFileContents(vertexShaderPath);
        ASSERT(!vsString.empty(), "Vertex shader source is empty");
        const char* vertexShaderSource = vsString.c_str();
        int vertexShader = glCreateShader(GL_VERTEX_SHADER);
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

        // fragment shader
        int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        std::string fsString = readFileContents(fragmentShaderPath);
        const char* fragmentShaderSource = fsString.c_str();
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

        m_Id = shaderProgram;
    }

    void use()
    {
        glUseProgram(m_Id);
    }

    void setUniform4f(std::string name, float x, float y, float z, float w)
    {
        int uniformID = glGetUniformLocation(m_Id, name.c_str());
        glUniform4f(uniformID, x, y, z, w);
    }

    void setUniform2f(const std::string& name, float x, float y)
    {
        int uniformID = glGetUniformLocation(m_Id, name.c_str());
        if (uniformID != -1)
        {
            glUniform2f(uniformID, x, y);
        }
    }

    void setUniform1i(std::string name, int value)
    {
        int uniformID = glGetUniformLocation(m_Id, name.c_str());
        glUniform1i(uniformID, value);
    }

    void deleteProgram()
    {
        glDeleteProgram(m_Id);
    }

    int ID()
    {
        return m_Id;
    }
};

#endif //PROJECT_SHADER_H
