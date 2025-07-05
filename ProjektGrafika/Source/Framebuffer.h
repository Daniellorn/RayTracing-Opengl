#pragma once

#include <cstdint>
#include <memory>

#include "Texture.h"

class Framebuffer
{
public:

	Framebuffer(std::shared_ptr<Texture> texture);

	void Bind() const;
	void UnBind() const;

private:

	bool AttachTextureToFramebuffer();

private:
	uint32_t m_FramebufferID;
	std::shared_ptr<Texture> m_Texture = nullptr;
};