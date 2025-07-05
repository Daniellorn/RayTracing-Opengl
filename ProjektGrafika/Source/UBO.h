#pragma once

#include <glad/glad.h>

#include <cstdint>
#include <vector>
#include <cassert>

template<typename T>
class UBO
{
public:

	UBO(const std::vector<T>& data, int slot):
		m_Slot(slot), m_DataSize(data.size())
	{
		m_Data = new T[m_DataSize];
		memcpy(m_Data, data.data(), m_DataSize * sizeof(T));

		glGenBuffers(1, &m_BufferID);
		glBindBuffer(GL_UNIFORM_BUFFER, m_BufferID);
		glBufferData(GL_UNIFORM_BUFFER, m_DataSize * sizeof(T), m_Data, GL_STREAM_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, m_Slot, m_BufferID);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	~UBO()
	{
		glDeleteBuffers(1, &m_BufferID);
		delete[] m_Data;
	}

	void Update(std::vector<T>& updatedData)
	{
		assert(updatedData.size() == m_DataSize);

		glBindBuffer(GL_UNIFORM_BUFFER, m_BufferID);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, m_DataSize * sizeof(T), updatedData.data());
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

private:
	uint32_t m_BufferID;
	uint32_t m_Slot;
	std::size_t m_DataSize;
	T* m_Data = nullptr;
};


