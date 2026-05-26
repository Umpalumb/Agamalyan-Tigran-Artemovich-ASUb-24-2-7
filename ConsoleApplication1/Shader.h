#ifndef SHADER_H
#define SHADER_H

#include <gl/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

class Shader {
public:
    GLuint ProgramID;

    Shader(const char* vertexPath, const char* fragmentPath) {
        std::string vCode = readFile(vertexPath);
        std::string fCode = readFile(fragmentPath);

        const char* vShaderSource = vCode.c_str();
        const char* fShaderSource = fCode.c_str();

        GLuint vertex = compile(vShaderSource, GL_VERTEX_SHADER, "VERTEX");
        GLuint fragment = compile(fShaderSource, GL_FRAGMENT_SHADER, "FRAGMENT");

        ProgramID = glCreateProgram();
        glAttachShader(ProgramID, vertex);
        glAttachShader(ProgramID, fragment);
        glLinkProgram(ProgramID);
        validate(ProgramID, "PROGRAM");

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void use() {
        glUseProgram(ProgramID);
    }

    void activate() { glUseProgram(ProgramID); }

    void setRGBA(const std::string& name, float r, float g, float b, float a) {
        glUniform4f(glGetUniformLocation(ProgramID, name.c_str()), r, g, b, a);
    }

    void setVec3(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(glGetUniformLocation(ProgramID, name.c_str()), 1, &value[0]);
    }

    void setVec3(const std::string& name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(ProgramID, name.c_str()), x, y, z);
    }

    void setFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(ProgramID, name.c_str()), value);
    }

    void setMat4(const std::string& name, const glm::mat4& mat) {
        glUniformMatrix4fv(glGetUniformLocation(ProgramID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }

    void setMat3(const std::string& name, const glm::mat3& mat) {
        glUniformMatrix3fv(glGetUniformLocation(ProgramID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }

private:
    std::string readFile(const char* path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cout << "ÎØÈÁÊÀ: Íå óäàëîñü îòêðûòü ôàéë øåéäåðà: " << path << std::endl;
        }
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }

    GLuint compile(const char* source, GLenum type, std::string name) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);
        validate(shader, name);
        return shader;
    }

    void validate(GLuint element, std::string type) {
        GLint success;
        GLchar infoLog[1024];

        if (type != "PROGRAM") {
            glGetShaderiv(element, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(element, 1024, NULL, infoLog);
                std::cout << "ÎØÈÁÊÀ_ÊÎÌÏÈËßÖÈÈ_" << type << ":\n" << infoLog << std::endl;
            }
        }
        else {
            glGetProgramiv(element, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(element, 1024, NULL, infoLog);
                std::cout << "ÎØÈÁÊÀ_ËÈÍÊÎÂÊÈ_ÏÐÎÃÐÀÌÌÛ:\n" << infoLog << std::endl;
            }
        }



    };
    
};
#endif