#ifndef IMAGE_HPP
#define IMAGE_HPP

class Image {
public:
    Image(const char *image_path) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        SDL_Surface *surface = IMG_Load(image_path);

        if (surface->pixels) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
            glGenerateMipmap(GL_TEXTURE_2D);

            SDL_FreeSurface(surface);
        }
        else {
            std::cerr << "Falha ao carregar imagem: " << image_path << std::endl;
        }
    }

    void use() const {
        glBindTexture(GL_TEXTURE_2D, texture);
    }
protected:
    unsigned int texture = 0u;
};

#endif