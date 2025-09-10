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

	bool AttachTextureToFramebuffer(std::shared_ptr<Texture> texture);

private:
	uint32_t m_FramebufferID;
	std::shared_ptr<Texture> m_Texture = nullptr;
};