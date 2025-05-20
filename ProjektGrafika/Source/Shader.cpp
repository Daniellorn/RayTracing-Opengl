#include <glad/gl.h>
#include <fstream>
#include <sstream>
#include <limits>
#include <vector>
#include <iostream>

#include "Shader.h"

Shader::Shader(const std::filesystem::path& vertSrc, const std::filesystem::path& fragSrc)
{
	m_VertexShader = ParseShader(vertSrc);
	m_FragmentShader = ParseShader(fragSrc);

	m_RendererID = CreateShader(m_VertexShader, m_FragmentShader);
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

std::string Shader::ParseShader(const std::filesystem::path& filepath)
{
	std::ifstream file(filepath);

	if (!file)
	{
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

	glShaderSource(id, 1, &src, 0);
	glCompileShader(id);

	int isCompiled = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		int maxLength = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<char> infoLog(maxLength);
		glGetShaderInfoLog(id, maxLength, &maxLength, &infoLog[0]);

		glDeleteShader(id);

		std::cerr << "Shader compilation failed: " << std::string(infoLog.begin(), infoLog.end()) << std::endl;

		return std::numeric_limits<uint32_t>::max();
	}



	return id;
}

uint32_t Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	uint32_t program = glCreateProgram();
	uint32_t vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	uint32_t fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);


	return program;
}
