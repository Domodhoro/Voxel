#ifndef TEXTURE_HPP
#define TEXTURE_HPP

class Texture {
public:
    Texture(const char *texturePath) {
        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        SDL_Surface *surface = IMG_Load(texturePath);

        if (surface) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
            glGenerateMipmap(GL_TEXTURE_2D);

            SDL_FreeSurface(surface);
        }
        else {
            std::cerr << "Falha ao carregar textura: " << texturePath << std::endl;
        }
    }

    void bind() const {
        glBindTexture(GL_TEXTURE_2D, ID);
    }
protected:
    unsigned int ID = 0u;
};

#endif
