#include "Framebuffer.h"

#include <glad/glad.h>
#include <iostream>


Framebuffer::Framebuffer(std::shared_ptr<Texture> texture):
    m_Texture(texture)
{
	glCreateFramebuffers(1, &m_FramebufferID);

    if (!AttachTextureToFramebuffer())
    {
        glDeleteFramebuffers(1, &m_FramebufferID);
        std::exit(-1);
    }
}

void Framebuffer::Bind() const
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FramebufferID);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // default framebuffer (ekran)
    glBlitFramebuffer(0, 0, m_Texture->GetWidth(), m_Texture->GetHeight(),
        0, 0, m_Texture->GetWidth(), m_Texture->GetHeight(),
        GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void Framebuffer::UnBind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool Framebuffer::AttachTextureToFramebuffer()
{
    auto TexID = m_Texture->GetTextureID();

    glNamedFramebufferTexture(m_FramebufferID, GL_COLOR_ATTACHMENT0, TexID, 0);

    auto fbostatus = glCheckNamedFramebufferStatus(m_FramebufferID, GL_FRAMEBUFFER);
    if (fbostatus != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Framebuffer error " << fbostatus << std::endl;
        return false;
    }

    return true;
}
