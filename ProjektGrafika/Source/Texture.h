#pragma once

#include <cstdint>

struct Texture
{
	uint32_t textureID;
	int width, height;
};

struct Framebuffer
{
	uint32_t framebufferID;
	Texture frameBufferTex;
};

Texture CreateTexture(int width, int height);

Framebuffer CreateFramebuffer(const Texture texture);
bool AttachTextureToFramebuffer(Framebuffer& fb, const Texture texture);
void BlitFrambuffer(const Framebuffer fb);
