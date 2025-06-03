#pragma once

#include <string>
#include <filesystem>
#include <unordered_map>

#include <glm/mat4x4.hpp>


class Shader
{
public: 

	Shader(const std::filesystem::path& computeSrc);
	~Shader();

	void Bind() const;
	void UnBind() const;

	void SetUniform1i(const std::string& name, int value);
	void SetUniform1f(const std::string& name, float value);
	void SetUniform3f(const std::string& name, float v0, float v1, float v2);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);

	void SetUniformMat4fm(const std::string& name, const glm::mat4& matrix);

private:
	
	std::string ParseShader(const std::filesystem::path& filepath);
	uint32_t CompileShader(uint32_t type, const std::string& source);
	uint32_t CreateShader(const std::string& computeShader);
	int GetUniformLocation(const std::string& name);

private:
	uint32_t m_RendererID;

	std::string m_ComputeShader;
	std::unordered_map<std::string, int> m_UniformLocationCache;
};