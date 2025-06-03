#include <glad/gl.h>
#include <fstream>
#include <sstream>
#include <limits>
#include <vector>

#include "Shader.h"
#include "Assert.h"

Shader::Shader(const std::filesystem::path& computeSrc)
{
	m_ComputeShader = ParseShader(computeSrc);
	m_RendererID = CreateShader(m_ComputeShader);
}

Shader::~Shader()
{
	glDeleteProgram(m_RendererID);
}

void Shader::Bind() const
{
	glUseProgram(m_RendererID);
}

void Shader::UnBind() const
{
	glUseProgram(0);
}

void Shader::SetUniform1i(const std::string& name, int value)
{
	glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform1f(const std::string& name, float value)
{
	glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
	glUniform3f(GetUniformLocation(name), v0, v1, v2);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}


void Shader::SetUniformMat4fm(const std::string& name, const glm::mat4& matrix)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

std::string Shader::ParseShader(const std::filesystem::path& filepath)
{
	std::ifstream file(filepath);

	if (!file)
	{
		ASSERT(std::filesystem::exists(filepath));
		return "";
	}

	std::ostringstream content;

	content << file.rdbuf();

	return content.str();
}

uint32_t Shader::CompileShader(uint32_t type, const std::string& source)
{
	uint32_t id = glCreateShader(type);
	const char* src = source.c_str();

	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int isCompiled = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		int maxLength = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<char> infoLog(maxLength);
		glGetShaderInfoLog(id, maxLength, &maxLength, infoLog.data());

		glDeleteShader(id);

		std::cerr << "Shader compilation failed: " << std::string(infoLog.begin(), infoLog.end()) << std::endl;

		return std::numeric_limits<uint32_t>::max();
	}



	return id;
}

uint32_t Shader::CreateShader(const std::string& computeShader)
{
	uint32_t program = glCreateProgram();
	uint32_t cs = CompileShader(GL_COMPUTE_SHADER, computeShader);

	if (cs == std::numeric_limits<uint32_t>::max())
	{
		glDeleteProgram(program);
		return std::numeric_limits<uint32_t>::max();
	}

	glAttachShader(program, cs);
	glLinkProgram(program);

	int isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);

	if (isLinked == GL_FALSE)
	{
		int maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<char> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());

		std::cerr << "Shader linking failed: " << std::string(infoLog.begin(), infoLog.end()) << std::endl;

		glDeleteProgram(program);
		return std::numeric_limits<uint32_t>::max();
	}

	glDetachShader(program, cs);
	glDeleteShader(cs);


	return program;
}

int Shader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
	{
		return m_UniformLocationCache[name];
	}

	int location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1)
	{
		std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
	}

	m_UniformLocationCache[name] = location;
	return location;
}
