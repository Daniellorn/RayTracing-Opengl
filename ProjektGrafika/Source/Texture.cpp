#include "Texture.h"

#include <glad/gl.h>

Texture::Texture(int width, int height) :
    m_TextureID(0), m_width(width), m_height(height)
{
    CreateTexture();
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_TextureID);
}

void Texture::Bind(int slot) const
{
    glBindImageTexture(slot, m_TextureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}

void Texture::Resize(int width, int height)
{
    if (m_width != width || m_height != height)
    {
        if (m_TextureID)
        {
            glDeleteTextures(1, &m_TextureID);
        }

        CreateTexture();

        m_width = width;
        m_height = height;
    }
}

uint32_t Texture::GetTextureID() const
{
    return m_TextureID;
}

void Texture::CreateTexture()
{
    glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);

    glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(m_TextureID, 1, GL_RGBA32F, m_width, m_height);
}
