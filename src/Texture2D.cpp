#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Texture2D.h"
#include <iostream>

Texture2D::Texture2D(const std::string &imagePath)
    : _path(imagePath)
{
    stbi_set_flip_vertically_on_load(true);

    _data = stbi_load(_path.c_str(), &_w, &_h, &_c, 0);
    if (!_data) {
        std::cerr << "[Texture2D] Failed to load: " << _path << std::endl;
    }
}

Texture2D::~Texture2D() {
    if (_data) {
        stbi_image_free(_data);
        _data = nullptr;
    }

    if (_texID) {
        glDeleteTextures(1, &_texID);
    }
}


void Texture2D::initializeGL() {
    if (!_data) return;  // 加载失败就跳过

    // 1. 创建并绑定纹理
    glGenTextures(1, &_texID);
    glBindTexture(GL_TEXTURE_2D, _texID);

    // 2. 设置采样参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 3. 根据通道数选择格式
    GLenum fmt = GL_RGB;
    if (_c == 1)      fmt = GL_RED;
    else if (_c == 3) fmt = GL_RGB;
    else if (_c == 4) fmt = GL_RGBA;

    // 4. 上传到 GPU 并生成 Mipmap
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, _w, _h, 0, fmt, GL_UNSIGNED_BYTE, _data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // 5. 解绑
    glBindTexture(GL_TEXTURE_2D, 0);

    // 6. 释放 CPU 数据
    stbi_image_free(_data);
    _data = nullptr;
}

void Texture2D::bind(GLenum unit) const {
    glActiveTexture(unit);
    glBindTexture(GL_TEXTURE_2D, _texID);
}
