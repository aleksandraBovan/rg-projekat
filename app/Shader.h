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
#include <filesystem>
#include <chrono>

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
    std::string m_vsPath;
    std::string m_fsPath;
    std::filesystem::file_time_type m_vsTime;
    std::filesystem::file_time_type m_fsTime;

public:
    Shader(std::string vertexShaderPath, std::string fragmentShaderPath)
    {
        m_vsPath = vertexShaderPath;
        m_fsPath = fragmentShaderPath;
        m_vsTime = std::filesystem::last_write_time(m_vsPath);
        m_fsTime = std::filesystem::last_write_time(m_fsPath);

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

    void checkForChanges()
    {
        auto newVsTime = std::filesystem::last_write_time(m_vsPath);
        auto newFsTime = std::filesystem::last_write_time(m_fsPath);

        if (newVsTime == m_vsTime && newFsTime == m_fsTime)
            return;

        m_vsTime = newVsTime;
        m_fsTime = newFsTime;

        std::string vsString = readFileContents(m_vsPath);
        std::string fsString = readFileContents(m_fsPath);

        const char* vSource = vsString.c_str();
        unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vSource, NULL);
        glCompileShader(vs);

        int success;
        char infoLog[512];
        glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vs, 512, NULL, infoLog);
            std::cerr << "VERTEX SHADER ERROR:\n" << infoLog << std::endl;
            glDeleteShader(vs);
            return; // Zadrzi stari program
        }

        const char* fSource = fsString.c_str();
        unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fSource, NULL);
        glCompileShader(fs);

        glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fs, 512, NULL, infoLog);
            std::cerr << "FRAGMENT SHADER ERROR:\n" << infoLog << std::endl;
            glDeleteShader(vs);
            glDeleteShader(fs);
            return;
        }

        unsigned int newProgram = glCreateProgram();
        glAttachShader(newProgram, vs);
        glAttachShader(newProgram, fs);
        glLinkProgram(newProgram);

        glGetProgramiv(newProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(newProgram, 512, NULL, infoLog);
            std::cerr << "LINK ERROR:\n" << infoLog << std::endl;
            glDeleteShader(vs);
            glDeleteShader(fs);
            glDeleteProgram(newProgram);
            return;
        }

        glDeleteShader(vs);
        glDeleteShader(fs);

        glDeleteProgram(m_Id);
        m_Id = newProgram;
    }
};

#endif //PROJECT_SHADER_H
