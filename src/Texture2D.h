#pragma once

#include "IRenderResource.h"
#include <string>
#include <vector>
#include <glad/glad.h>

class Texture2D : public IRenderResource {
public:
    /// 构造时只读取文件到 CPU 内存，不做任何 GL 的操作
    explicit Texture2D(const std::string& imagePath);
    Texture2D(const std::string& identifier, const unsigned char* data, int len);
    ~Texture2D() override;

    void initializeGL() override;
    
    void bind(GLenum unit = GL_TEXTURE0) const;

    GLuint getID() const { return _texID; }

private:
    std::string _path;
    int _w = 0;
    int _h = 0;
    int _c = 0;
    
    unsigned char* _dataFromFile = nullptr;
    // std::vector<unsigned char> _dataFromMemory;

    GLuint _texID = 0;
};