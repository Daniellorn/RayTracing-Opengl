#pragma once

#include <cstdint>

class Texture
{
public:

	Texture(int width, int height);
	~Texture();

	void Bind(int slot) const;
	void Resize(int width, int height);

	uint32_t GetTextureID() const;

private:

	void CreateTexture();

private:

	uint32_t m_TextureID;

	int m_width, m_height;

};