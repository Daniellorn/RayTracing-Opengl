#pragma once

#include <cstdint>
#include <string>
#include <filesystem>

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
uint32_t CreateCubeMap(std::filesystem::path filepath);

Framebuffer CreateFramebuffer(const Texture texture);
bool AttachTextureToFramebuffer(Framebuffer& fb, const Texture texture);
void BlitFrambuffer(const Framebuffer fb);
