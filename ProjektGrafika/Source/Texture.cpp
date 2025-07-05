#include "Texture.h"

#include <glad/glad.h>
#include <iostream>

#include "stb_image.h"

Texture::Texture(int width, int height):
    m_Width(width), m_Height(height), m_BindingCounter(s_Counter++)
{

    glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);

    glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(m_TextureID, 1, GL_RGBA32F, width, height);

}

Texture::Texture(std::filesystem::path filepath):
    m_BindingCounter(s_Counter++)
{
    int width, height, nrComponents;

    float* data = stbi_loadf(filepath.string().c_str(), &width, &height, &nrComponents, 0);
    if (!data)
    {
        std::cerr << "Failed to load HDR image\n";
        std::exit(-1);
    }

    glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
    glTextureStorage2D(m_TextureID, 1, GL_RGB32F, width, height);

    glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureSubImage2D(m_TextureID, 0, 0, 0, width, height, GL_RGB, GL_FLOAT, data);

    stbi_image_free(data);

    m_Width = width;
    m_Height = height;

    glBindTextureUnit(m_BindingCounter, m_TextureID);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_TextureID);
}

//void Texture::Resize(int width, int height)
//{
//    glDeleteTextures(1, &m_TextureID);
//}

void Texture::Bind() const
{
    glBindImageTexture(m_BindingCounter, m_TextureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}
