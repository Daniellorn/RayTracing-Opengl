#include "Texture.h"

#include <glad/glad.h>
#include <iostream>


Texture CreateTexture(int width, int height)
{
    Texture tex{
        .width = width,
        .height = height
    };

    glCreateTextures(GL_TEXTURE_2D, 1, &tex.textureID);

    glTextureParameteri(tex.textureID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(tex.textureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureParameteri(tex.textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(tex.textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(tex.textureID, 1, GL_RGBA32F, width, height);


    return tex;
}

Framebuffer CreateFramebuffer(const Texture texture)
{
    Framebuffer fb;
    fb.frameBufferTex = texture;

    glCreateFramebuffers(1, &fb.framebufferID);

    if (!AttachTextureToFramebuffer(fb, texture))
    {
        glDeleteFramebuffers(1, &fb.framebufferID);
        return {};
    }

    return fb;
}

bool AttachTextureToFramebuffer(Framebuffer& fb, const Texture texture)
{
    glNamedFramebufferTexture(fb.framebufferID, GL_COLOR_ATTACHMENT0, texture.textureID, 0);

    auto fbostatus = glCheckNamedFramebufferStatus(fb.framebufferID, GL_FRAMEBUFFER);
    if (fbostatus != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Framebuffer error" << std::endl;
        return false;
    }
    fb.frameBufferTex = texture;

    return true;
}

void BlitFrambuffer(const Framebuffer fb)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fb.framebufferID);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // default framebuffer (ekran)
    glBlitFramebuffer(0, 0, fb.frameBufferTex.width, fb.frameBufferTex.height,
        0, 0, fb.frameBufferTex.width, fb.frameBufferTex.height,
        GL_COLOR_BUFFER_BIT, GL_NEAREST);
}


