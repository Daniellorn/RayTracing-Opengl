#pragma once

#include <string>
#include <filesystem>


class Shader
{
public: 

	Shader(const std::filesystem::path& computeSrc);
	~Shader();

	void Bind() const;
	void UnBind() const;

private:
	
	std::string ParseShader(const std::filesystem::path& filepath);
	uint32_t CompileShader(uint32_t type, const std::string& source);
	uint32_t CreateShader(const std::string& computeShader);

private:
	uint32_t m_RendererID;

	std::string m_ComputeShader;

};