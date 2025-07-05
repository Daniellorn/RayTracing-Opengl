#pragma once

#include <cstdint>
#include <string>
#include <filesystem>


class Texture
{
public:
	Texture() = default;
	Texture(int width, int height);
	Texture(std::filesystem::path filepath);
	~Texture();

	uint32_t GetTextureID() const { return m_TextureID; }
	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }

	//void Resize(int width, int height);

	void Bind() const;

private:
	uint32_t m_TextureID;
	int m_Width, m_Height;

	int m_BindingCounter;
	inline static int s_Counter = 0;
};
