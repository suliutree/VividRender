#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Texture2D.h"
#include <iostream>

Texture2D::Texture2D(const std::string &imagePath)
    : _path(imagePath)
{
    stbi_set_flip_vertically_on_load(true);

    _dataFromFile = stbi_load(_path.c_str(), &_w, &_h, &_c, 0);
    if (!_dataFromFile) {
        std::cerr << "[Texture2D] Failed to load from file: " << _path << std::endl;
    }
}

Texture2D::Texture2D(const std::string& identifier, const unsigned char* data, int len)
    : _path(identifier)
{
    stbi_set_flip_vertically_on_load(true);
    _dataFromFile = stbi_load_from_memory(data, len, &_w, &_h, &_c, 0);
    if (!_dataFromFile) {
        std::cerr << "[Texture2D] Failed to load from memory: " << identifier << std::endl;
    }
}

Texture2D::~Texture2D() {
    if (_dataFromFile) {
        stbi_image_free(_dataFromFile);
        _dataFromFile = nullptr;
    }

    if (_texID) {
        // GL 资源应该在渲染线程中销毁，这里只是一个示例
        // 在一个完整的引擎中，需要一个延迟销毁队列
        glDeleteTextures(1, &_texID);
    }
}


void Texture2D::initializeGL() {
    if (!_dataFromFile) return;  // 加载失败就跳过

    glGenTextures(1, &_texID);
    glBindTexture(GL_TEXTURE_2D, _texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum fmt = GL_RGB;
    if (_c == 1)      fmt = GL_RED;
    else if (_c == 3) fmt = GL_RGB;
    else if (_c == 4) fmt = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, fmt, _w, _h, 0, fmt, GL_UNSIGNED_BYTE, _dataFromFile);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(_dataFromFile);
    _dataFromFile = nullptr;
}

void Texture2D::bind(GLenum unit) const {
    glActiveTexture(unit);
    glBindTexture(GL_TEXTURE_2D, _texID);
}
