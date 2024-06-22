#ifndef SHADER_HPP
#define SHADER_HPP

class Shader {
public:
    Shader(const char *vertexShaderSource, const char *fragmentShaderSource) {
        unsigned int vertex = glCreateShader(GL_VERTEX_SHADER), fragment = glCreateShader(GL_FRAGMENT_SHADER);
        int success = 0;

        glShaderSource(vertex, 1, &vertexShaderSource, nullptr);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

        if (!success) {
            char infoLog[512];
            
            glGetShaderInfoLog(vertex, 512, nullptr, infoLog);

            std::cerr << infoLog << std::endl;
        }

        glShaderSource(fragment, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        
        if (!success) {
            char infoLog[512];

            glGetShaderInfoLog(fragment, 512, nullptr, infoLog);

            std::cerr << infoLog << std::endl;
        }

        ID = glCreateProgram();

        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        glGetProgramiv(ID, GL_LINK_STATUS, &success);

        if (!success) {
            char infoLog[512];

            glGetProgramInfoLog(ID, 512, nullptr, infoLog);

            std::cerr << infoLog << std::endl;
        }

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void use() const {
        glUseProgram(ID);
    }

    void setBool(const char *name, bool value) const { 
        glUniform1i(glGetUniformLocation(ID, name), static_cast<int>(value));
    }

    void setInt(const char *name, int value) const { 
        glUniform1i(glGetUniformLocation(ID, name), value); 
    }

    void setFloat(const char *name, float value) const { 
        glUniform1f(glGetUniformLocation(ID, name), value);
    }

    void setVec2(const char *name, const glm::vec2 vector) const { 
        glUniform2fv(glGetUniformLocation(ID, name), 1, glm::value_ptr(vector));
    }

    void setVec3(const char *name, const glm::vec3 vector) const { 
        glUniform3fv(glGetUniformLocation(ID, name), 1, glm::value_ptr(vector)); 
    }

    void setVec4(const char *name, const glm::vec4 vector) const { 
        glUniform4fv(glGetUniformLocation(ID, name), 1, glm::value_ptr(vector)); 
    }

    void setMat2(const char *name, const glm::mat2 matrix) const { 
        glUniformMatrix2fv(glGetUniformLocation(ID, name), 1, false, glm::value_ptr(matrix));
    }

    void setMat3(const char *name, const glm::mat3 matrix) const { 
        glUniformMatrix3fv(glGetUniformLocation(ID, name), 1, false, glm::value_ptr(matrix));
    }

    void setMat4(const char *name, const glm::mat4 matrix) const { 
        glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, false, glm::value_ptr(matrix));
    }
protected:
    unsigned int ID = 0u;
};

#endif