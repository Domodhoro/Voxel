#ifndef TEXTURE_HPP
#define TEXTURE_HPP

class Texture {
public:
    void load(const std::string &texturePath, const std::string &textureName) {
        u_int textureID = 0u;

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        SDL_Surface *surface = IMG_Load(texturePath.c_str());

        if (surface) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
            glGenerateMipmap(GL_TEXTURE_2D);

            SDL_FreeSurface(surface);

            textures[textureName] = textureID;
        } 
        else {
            std::cerr << "Falha ao carregar textura: " << texturePath << std::endl;
        }
    }

    void load(const std::vector<std::string> &faces, const std::string &textureName) {
        u_int textureID = 0u;

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        for (size_t i = 0; i < faces.size(); ++i) {
            SDL_Surface *surface = IMG_Load(faces[i].c_str());

            if (surface) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, surface->w, surface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
                SDL_FreeSurface(surface);
            } 
            else {
                std::cerr << "Falha ao carregar textura: " << faces[i] << std::endl;
            }
        }

        textures[textureName] = textureID;
    }

    void bind(const std::string &textureName, GLenum textureType = GL_TEXTURE_2D) const {
        auto it = textures.find(textureName);

        if (it != textures.end()) {
            glBindTexture(textureType, it->second);
        } 
        else {
            std::cerr << "Textura não encontrada: " << textureName << std::endl;
        }
    }

    void unbind(GLenum textureType = GL_TEXTURE_2D) const {
        glBindTexture(textureType, 0u);
    }
protected:
    std::map<std::string, u_int> textures;
};

#endif