#ifndef SHADER_PROGRAM_HPP
#define SHADER_PROGRAM_HPP

class ShaderProgram {
public:
    ShaderProgram(const std::string &vertexShaderSource, const std::string &fragmentShaderSource) {
        u_int vertex = compileShader(vertexShaderSource.c_str(), GL_VERTEX_SHADER);
        u_int fragment = compileShader(fragmentShaderSource.c_str(), GL_FRAGMENT_SHADER);

        int success = 0;

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
    u_int ID = 0u;

    u_int compileShader(const char *shaderSource, GLenum shaderType) {
        int success = 0;

        u_int shader = glCreateShader(shaderType);

        glShaderSource(shader, 1, &shaderSource, nullptr);
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success) {
            char infoLog[512];
            
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);

            std::cerr << infoLog << std::endl;
        }

        return shader;
    }
};

#endif